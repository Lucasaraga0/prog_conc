public class Bird extends Thread {
    private final HungryBirds monitor;
    private final int id;
    private final int timeSleep;

    public Bird(HungryBirds monitor, int id, int timeSleep) {
        this.monitor = monitor;
        this.id = id;
        this.timeSleep = timeSleep;
    }

    @Override
    public void run() {
        while (true) {
            monitor.getFood(id);
            try {
                Thread.sleep(50); // simula o tempo comendo
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
            monitor.releaseFood(id);

            try {
                Thread.sleep(timeSleep); // tempo dormindo
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }
    }
}
