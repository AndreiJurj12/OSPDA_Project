import datetime

import numpy as np
from mpi4py import MPI


def read_vector_from_file(filename: str):
    with open(filename) as f:
        n = int(f.readline())
        numbers = [int(number) for number in f.readline().split()]
    return n, numbers


def parent_process():
    comm = MPI.COMM_WORLD
    start_time = datetime.datetime.now()

    filename = "file_large.txt"
    n, numbers = read_vector_from_file(filename)
    numbers = np.array(numbers, dtype=np.int32)
    input_output_time = datetime.datetime.now()
    input_output_duration = (input_output_time - start_time).total_seconds() * (10 ** 6)  # microseconds
    print("Process rank #0: Reading the vector from file took {} microseconds".format(input_output_duration))

    total_no_processes = comm.Get_size() - 1  # eliminate parent process which doesn't process a chunk
    if total_no_processes == 0:
        exit(-1)  # no processes to compute

    chunk_length = n / total_no_processes
    current_process_rank = 1
    current_vector_starting_index = 0

    for i in range(total_no_processes):
        current_vector_ending_index = int(min(len(numbers) - 1, current_vector_starting_index + chunk_length))
        process_chunk_length = current_vector_ending_index - current_vector_starting_index + 1
        print("Starting index {}".format(current_vector_starting_index))
        print("Ending index {}".format(current_vector_ending_index))
        print("Sending length {}".format(process_chunk_length))


        # send values and so on
        comm.send(process_chunk_length, dest=current_process_rank, tag=0)
        comm.Send([numbers[current_vector_starting_index:current_vector_ending_index + 1], process_chunk_length, MPI.INT],
                  dest=current_process_rank, tag=0)

        current_process_rank += 1
        current_vector_starting_index += process_chunk_length

    total_sum = 0
    current_process_rank = 1
    for i in range(total_no_processes):
        received_sum = comm.recv(source=current_process_rank, tag=0)  # receive value
        total_sum += int(received_sum)

        current_process_rank += 1

    average_value = (1.0 * total_sum) / n
    print("Process rank #0: The average value of the vector is {}".format(average_value))

    final_time = datetime.datetime.now()
    computation_duration = (final_time - input_output_time).total_seconds() * (10 ** 6)
    total_duration = (final_time - start_time).total_seconds() * (10 ** 6)

    print("Process rank #0: Processing (computing time) took {} microseconds".format(computation_duration))
    print("Process rank #0: Total program duration was {} microseconds".format(total_duration))


def child_process(process_rank: int):
    comm = MPI.COMM_WORLD

    # receive length and then array
    length = comm.recv(source=0, tag=0)

    # initialize and receive array
    child_process_chunk = np.empty(length, dtype=np.int32)
    comm.Recv(child_process_chunk, source=0, tag=0)

    total_sum = 0
    for number in child_process_chunk:
        total_sum += int(number)

    print("Process with rank {} is sending back sum {}".format(process_rank, total_sum))
    comm.send(total_sum, dest=0, tag=0)


def main():
    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()
    if rank == 0:
        parent_process()
    else:
        child_process(rank)


if __name__ == "__main__":
    main()
