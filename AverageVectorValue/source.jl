

function read_vector_from_file(filename)
    open(filename) do io
        n = parse(Int, readline(io))
        numbers = [parse(Int, number) for number in split(readline(io))]
        return n, numbers
    end

end

function main()
    starting_unix_time = time()

    filename = "file_large.txt"
    if !isempty(ARGS)
        filename = ARGS[1]
    end
    n, numbers = read_vector_from_file(filename)

    input_output_time = time()
    input_output_duration = trunc(Int, (input_output_time - starting_unix_time) * (10^6))
    println("Reading the vector from file took $input_output_duration  microseconds")

    list_sum = 0
    for number in numbers
        list_sum += number
    end
    println("Total sum is $list_sum")

    average_value = (1.0 * list_sum) / n
    println("The average value of the vector is $average_value")
    final_time = time()
    computation_duration = trunc(Int, (final_time - input_output_time) * (10^6))
    total_duration = trunc(Int, (final_time - starting_unix_time) * (10^6))

    println("Processing (computing time) took $computation_duration microseconds")
    println("Total program duration was $total_duration microseconds")

end


main()



