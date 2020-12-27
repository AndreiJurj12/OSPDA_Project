import numpy as np

from absl import app, flags
from absl.flags import FLAGS
from random import randint


flags.DEFINE_integer('n', 10, 'Vector size to generate')
flags.DEFINE_string('filename', 'test.txt', 'filename to save the vector to')


def main(_argv):
    n = FLAGS.n
    filename = FLAGS.filename
    
    vector = np.zeros(n)
    for i in range(n):
        vector[i] = int(randint(0, 1000))
    
    
    with open(filename, "w") as f:
        f.write("{}\n".format(n))
        output_buffer = " ".join(str(int(item)) for item in vector)
        f.write(output_buffer)
        
    print("done")


if __name__ == "__main__":
    try:
        app.run(main)
    except SystemExit:
        pass