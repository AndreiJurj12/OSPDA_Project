import mpi.MPI;

import java.io.File;
import java.io.FileNotFoundException;
import java.time.Duration;
import java.time.Instant;
import java.util.ArrayList;
import java.util.Scanner;

import static java.lang.Math.min;

/*Java is so bad that you can receive a single value..create an array for that
    Also for sending ...........................................
 */
public class Source_MPI {

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

    private static void parentProcess() {
        Instant startingTime = Instant.now();
        String filename = "file_large.txt";

        ArrayList<Integer> numbers = readVectorFromFile(filename);
        int n = numbers.size();
        Instant inputOutputTime = Instant.now();
        Duration inputOutputDuration = Duration.between(startingTime, inputOutputTime);
        System.out.println(String.format("Reading the vector from file took %s  microseconds", String.valueOf(inputOutputDuration.toNanos() / 1000)));

        int totalNoProcesses = MPI.COMM_WORLD.Size() - 1;
        if (totalNoProcesses == 0) {
            System.exit(-1);
        }


        int chunkLength = n / totalNoProcesses;
        int currentProcessRank = 1;
        int currentVectorStartingIndex = 0;
        int[] numbersArray = numbers.stream().mapToInt(x -> x).toArray();

        for (int i = 0; i < totalNoProcesses; i += 1) {
            int currentVectorEndingIndex = min(numbers.size() - 1, currentVectorStartingIndex + chunkLength);
            int processChunkLength = currentVectorEndingIndex - currentVectorStartingIndex + 1;

            int[] processChunkLengthArray = new int[1];
            processChunkLengthArray[0] = processChunkLength;


            MPI.COMM_WORLD.Ssend(processChunkLengthArray, 0, 1, MPI.INT, currentProcessRank, 0);
            MPI.COMM_WORLD.Ssend(numbersArray, currentVectorStartingIndex, processChunkLength, MPI.INT, currentProcessRank, 0);

            currentProcessRank += 1;
            currentVectorStartingIndex += processChunkLength ;
        }

        long totalSum = 0;
        currentProcessRank = 1;
        for (int i = 0; i < totalNoProcesses; i += 1) {
            //Java is so bad that you can receive a single value..create an array for that
            long[] receivedSumArray = new long[1];
            MPI.COMM_WORLD.Recv(receivedSumArray, 0, 1, MPI.LONG, currentProcessRank, MPI.ANY_TAG);
            totalSum += receivedSumArray[0];

            currentProcessRank += 1;
        }

        double average_value = (1.0 * totalSum) / numbers.size();
        System.out.println("Process rank #0: The average value of the vector is " + average_value);

        Instant finalTime = Instant.now();
        Duration computationDuration = Duration.between(inputOutputTime, finalTime);
        Duration totalDuration = Duration.between(startingTime, finalTime);

        System.out.println(String.format("Process rank #0: Processing (computing time) took %s microseconds", String.valueOf(computationDuration.toNanos() / 1000)));
        System.out.println(String.format("Process rank #0: Total program duration was %s microseconds", String.valueOf(totalDuration.toNanos() / 1000)));

    }

    private static void childProcess(int processRank) {
        //Java is so bad that you can receive a single value..create an array for that
        int[] lengthArray = new int[1];
        MPI.COMM_WORLD.Recv(lengthArray, 0, 1, MPI.INT, 0, MPI.ANY_TAG);
        int length = lengthArray[0];
        System.out.println("Length received is:" + length);

        //ArrayList<Integer> childProcessChunk = new ArrayList<>(length); // Java is not like C++ :((
        int[] childProcessChunk = new int[length];
        MPI.COMM_WORLD.Recv(childProcessChunk, 0, length, MPI.INT, 0, MPI.ANY_TAG);

        long chunkSum = 0;
        for(Integer number: childProcessChunk) {
            chunkSum += number;
        }

        System.out.println(String.format("Process rank #%d is sending sum %s back", processRank, String.valueOf(chunkSum)));

        long[] chunkSumArray = new long[1];
        chunkSumArray[0] = chunkSum;
        MPI.COMM_WORLD.Ssend(chunkSumArray, 0, 1, MPI.LONG, 0, 0);

    }

    public static void main(String[] args) {
        MPI.Init(args);
        int rank = MPI.COMM_WORLD.Rank();
        if (rank == 0) {
            parentProcess();
        }
        else {
            childProcess(rank);
        }
        MPI.Finalize();
    }
}
