import datetime

from absl import app, flags
from absl.flags import FLAGS

flags.DEFINE_string('filename', 'file_large.txt', 'filename to save the vector to')


def read_vector_from_file(filename: str):
    with open(filename) as f:
        n = int(f.readline())
        numbers = [int(number) for number in f.readline().split()]
    return n, numbers


def main(_argv):
    start_time = datetime.datetime.now()

    n, numbers = read_vector_from_file(FLAGS.filename)
    input_output_time = datetime.datetime.now()
    input_output_duration = (input_output_time - start_time).total_seconds() * (10**6)  # microseconds
    print("Reading the vector from file took {} microseconds".format(input_output_duration))

    list_sum = 0
    for number in numbers:
        list_sum += number
    print("Total sum is {}".format(list_sum))
    average_value = (1.0 * list_sum) / n
    print("The average value of the vector is {}".format(average_value))

    final_time = datetime.datetime.now()
    computation_duration = (final_time - input_output_time).total_seconds() * (10**6)
    total_duration = (final_time - start_time).total_seconds() * (10**6)

    print("Processing (computing time) took {} microseconds".format(computation_duration))
    print("Total program duration was {} microseconds".format(total_duration))


if __name__ == "__main__":
    try:
        app.run(main)
    except SystemExit:
        pass
