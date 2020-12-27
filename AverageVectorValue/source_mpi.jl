using MPI


function read_vector_from_file(filename::String)
    open(filename) do io
        n = parse(Int, readline(io))
        numbers = vec([parse(Int, number) for number in split(readline(io))])
        return n, numbers
    end
end

function parent_process()
    comm = MPI.COMM_WORLD
    starting_unix_time = time()

    filename = "file_large.txt"
    n, numbers = read_vector_from_file(filename)
    # println(typeof(numbers))



    input_output_time = time()
    input_output_duration = trunc(Int, (input_output_time - starting_unix_time) * (10^6))
    println("Process rank #0: Reading the vector from file took $input_output_duration microseconds")

    total_no_processes = MPI.Comm_size(comm) - 1 # eliminate parent process which doesn't process a chunk
    if total_no_processes == 0
        exit(-1)
    end

    numbers = Array{Int32}(numbers)
    chunk_length = trunc(Int, n / total_no_processes)
    current_process_rank::Integer = 1
    current_vector_starting_index::Integer = 1

    for i in 1:total_no_processes
        current_vector_ending_index = min(size(numbers)[1], current_vector_starting_index + chunk_length)
        process_chunk_length::Int32 = current_vector_ending_index - current_vector_starting_index + 1

        println("Process is $i")
        println("Starting index is $current_vector_starting_index")
        println("Ending index is $current_vector_ending_index")
        println("Process chunk length is $process_chunk_length")

        # send values
        MPI.send(process_chunk_length, current_process_rank, 0, comm)
        MPI.Send(numbers[current_vector_starting_index:current_vector_ending_index], current_process_rank, 0, comm)

        current_process_rank += 1
        current_vector_starting_index += process_chunk_length
    end

    total_sum::Int64 = 0
    current_process_rank = 1
    for i in 1:total_no_processes
        received_sum::Int64, _ = MPI.recv(current_process_rank, 0, comm)  # receive value
        total_sum += received_sum

        current_process_rank += 1
    end

    average_value = (1.0 * total_sum) / n
    println("Process rank #0: The average value of the vector is $average_value")

    final_time = time()
    computation_duration = trunc(Int, (final_time - input_output_time) * (10^6))
    total_duration = trunc(Int, (final_time - starting_unix_time) * (10^6))

    println("Process rank #0: Processing (computing time) took $computation_duration microseconds")
    println("Process rank #0: Total program duration was $total_duration microseconds")


end

function child_process(process_rank::Integer)
    comm = MPI.COMM_WORLD


    # receive length and then array
    length::Int32, _ = MPI.recv(0, 0, comm)
    # println("Length received is $length")

    # initialize and receive array
    child_process_chunk = zeros(Int32, length)
    MPI.Recv!(child_process_chunk, 0, 0, comm)
    vector_size = size(child_process_chunk)[1]
    # println("Received array of length $vector_size")

    total_sum = 0
    for number in child_process_chunk
        total_sum += number
    end

    println("Process with rank $process_rank is sending back sum $total_sum")
    MPI.send(total_sum, 0, 0, comm)
end

function main()
    MPI.Init()
    comm = MPI.COMM_WORLD
    rank = MPI.Comm_rank(comm)
    if rank == 0
        parent_process()
    else
        child_process(rank)
    end
    MPI.Finalize()
end


main()
