public class Parent extends Thread {
    private final HungryBirds monitor;

    public Parent(HungryBirds monitor) {
        this.monitor = monitor;
    }

    @Override
    public void run() {
        while (true) {
            monitor.restockFood();
        }
    }
}
