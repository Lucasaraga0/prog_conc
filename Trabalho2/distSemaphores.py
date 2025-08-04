from multiprocessing import Process, Queue
from enum import Enum
import time
import random
import queue

N = 6
NUM_ACESSOS = 3

class Kind(Enum):
    REQ_P = 1
    REQ_V = 2
    POP = 3
    VOP = 4
    ACK = 5

def insert_ordered(queue_list, item):
    timestamp = item[2]
    sender = item[0]

    pos = 0
    while (pos < len(queue_list)):
        ts = queue_list[pos][2]
        sender_pos = queue_list[pos][0]

        if (ts > timestamp):
            break
        elif (ts == timestamp and sender_pos > sender):
            break
        pos += 1
    queue_list.insert(pos, item)

def make_msg(sender, kind, timestamp):
    return {'sender': sender, 'kind': kind, 'timestamp': timestamp}

def make_ack(sender, ack_ts):
    return {'sender': sender, 'kind': Kind.ACK, 'timestamp': ack_ts}

def broadcast(queues, from_id, kind, timestamp):
    for i in range(N):
        msg = make_msg(from_id, kind, timestamp)
        queues[i].put(msg)

# processo usuario
def user(i, semop_queue, go_queue):
    lc = 0

    for _ in range(NUM_ACESSOS):
        semop_queue.put(make_msg(i, Kind.REQ_P, lc))
        lc += 1

        ts = go_queue.get()
        lc = max(lc, ts + 1)
        print(f"[Usuario {i}] acessou secao critica (lc={lc})")

        time.sleep(random.uniform(0.1, 0.3))

        semop_queue.put(make_msg(i, Kind.REQ_V, lc))
        lc += 1

# processo helper
def helper(i, semop_queue, go_queue, all_queues):
    lc = 0
    semaforo = 1
    mq = []  
    vc = [0] * N 

    while (True):
        try:
            msg = semop_queue.get(timeout=2)
        except queue.Empty:
            break

        sender = msg['sender']
        kind = msg['kind']
        ts = msg['timestamp']
        lc = max(lc, ts + 1)
        lc += 1

        if (kind == Kind.REQ_P):
            print(f"[Helper {i}] recebeu REQ_P de {sender}")
            broadcast(all_queues, i, Kind.POP, lc)

        elif (kind == Kind.REQ_V):
            print(f"[Helper {i}] recebeu REQ_V de {sender}")
            broadcast(all_queues, i, Kind.VOP, lc)

        elif (kind in (Kind.POP, Kind.VOP)):
            print(f"[Helper {i}] recebeu {kind.name} de {sender}")
            msg_key = (sender, kind, ts)
            insert_ordered(mq, msg_key)
            broadcast(all_queues, i, Kind.ACK, lc)

        elif (kind == Kind.ACK):
            ack_sender = sender
            ack_ts = ts
            vc[ack_sender] = max(vc[ack_sender], ack_ts)

        applied = True
        while (applied):
            applied = False
            for m in list(mq):
                msg_sender, msg_kind, msg_ts = m
                if (msg_ts <= min(vc)):  # fully acknowledged
                    if (msg_kind == Kind.VOP):
                        semaforo += 1
                        print(f"[Helper {i}] VOP aplicado -> {semaforo}")
                        mq.remove(m)
                        applied = True
                        break
                    elif (msg_kind == Kind.POP and semaforo > 0):
                        semaforo -= 1
                        print(f"[Helper {i}] POP aplicado -> {semaforo}")
                        if (msg_sender == i):
                            print(f"[Helper {i}] liberando processo {i}")
                            go_queue.put(lc)
                        mq.remove(m)
                        applied = True
                        break

if __name__ == '__main__':
    queues = [Queue() for _ in range(N)]
    go_queues = [Queue() for _ in range(N)]

    helpers = [
        Process(target=helper, args=(i, queues[i], go_queues[i], queues))
        for i in range(N)
    ]

    users = [
        Process(target=user, args=(i, queues[i], go_queues[i]))
        for i in range(N)
    ]

    for p in helpers + users:
        p.start()
    for p in helpers + users:
        p.join()