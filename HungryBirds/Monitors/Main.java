public class Main {
    public static void main(String[] args) {
        if (args.length != 3) {
            System.out.println("Uso: java Main <F> <N> <time_sleep_ms>");
            return;
        }

        int F = Integer.parseInt(args[0]);          // porções de comida
        int N = Integer.parseInt(args[1]);          // número de filhos
        int timeSleep = Integer.parseInt(args[2]);  // tempo de sono após comer

        HungryBirds monitor = new HungryBirds(F);

        for (int i = 1; i <= N; i++) {
            new Bird(monitor, i, timeSleep).start();
        }

        new Parent(monitor).start();
    }
}
