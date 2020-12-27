import java.io.File;
import java.io.FileNotFoundException;
import java.time.Duration;
import java.time.Instant;
import java.util.ArrayList;
import java.util.Scanner;

public class Source {

    private static ArrayList<Integer> readVectorFromFile(String filename) {
        ArrayList<Integer> numbers = new ArrayList<>();

        File file = new File(filename);
        try {
            Scanner scanner = new Scanner(file);
            int n = scanner.nextInt();
            for(int i = 0; i < n; i += 1) {
                int x = scanner.nextInt();
                numbers.add(x);
            }

        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }

        return numbers;
    }


    public static void main(String[] args) {
        Instant startingTime = Instant.now();

        String filename = "file_large.txt";
        if (args.length == 2) {
            filename = args[1];
        }

        ArrayList<Integer> numbers = Source.readVectorFromFile(filename);
        Instant inputOutputTime = Instant.now();
        Duration inputOutputDuration = Duration.between(startingTime, inputOutputTime);
        System.out.println(String.format("Reading the vector from file took %s  microseconds", String.valueOf(inputOutputDuration.toNanos() / 1000)));

        Long listSum = 0L;
        for (Integer number : numbers) {
            listSum += number;
        }
        System.out.println("Total sum is " + listSum.toString());

        double averageValue = (1.0 * listSum) / numbers.size();
        System.out.println("The average value of the vector is " + averageValue);
        Instant finalTime = Instant.now();
        Duration computationDuration = Duration.between(inputOutputTime, finalTime);
        Duration totalDuration = Duration.between(startingTime, finalTime);

        System.out.println(String.format("Processing (computing time) took %s microseconds", String.valueOf(computationDuration.toNanos() / 1000)));
        System.out.println(String.format("Total program duration was %s microseconds", String.valueOf(totalDuration.toNanos() / 1000)));
    }
}
