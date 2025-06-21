public class HungryBirds {
    private final int F;
    private int countFood;
    private boolean foodAccess = true;

    public HungryBirds(int F) {
        this.F = F;
        this.countFood = F;
    }

    public synchronized void getFood(int id) {
        while (!foodAccess) {
            try {
                wait();
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }
        foodAccess = false;
    }

    public synchronized void releaseFood(int id) {
        countFood--;
        System.out.printf("Filho %d comeu uma porção. Restam %d.\n", id, countFood);

        if (countFood == 0) {
            System.out.printf("Filho %d viu que acabou a comida. Acordando o pai.\n", id);
            notifyAll(); // avisa o pai
            try {
                wait(); // espera reabastecimento
                System.out.printf("Filho %d viu que o pai repôs a comida.\n", id);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }

        foodAccess = true;
        notifyAll(); // acorda os outros pássaros
    }

    public synchronized void restockFood() {
        while (countFood > 0) {
            try {
                wait(); // espera até a comida acabar
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }
        countFood = F;
        System.out.printf("Pai reabasteceu a comida com %d porções.\n", F);
        notifyAll(); // acorda os filhos
    }
}
