size_of_the_chessboard = int(input("Enter the size of the chessboard: "))
stack = [[]]
number_of_solutions = 0
while len(stack):
    current_attempt = stack.pop()
    if len(current_attempt) == size_of_the_chessboard:
        number_of_solutions += 1
        string_to_be_printed = ""
        for i in range(0, size_of_the_chessboard):
            string_to_be_printed += chr(ord('A') + i) + str(current_attempt[i] + 1) + (
                    " " if i < size_of_the_chessboard - 1 else ""
                    )
        print("Found a solution:", string_to_be_printed)
        string_to_be_printed = ""
        for i in range(0, size_of_the_chessboard):
            for j in range(0, size_of_the_chessboard):
                if current_attempt[j] == size_of_the_chessboard - i - 1:
                    string_to_be_printed += "Q"
                else:
                    string_to_be_printed += "."
            if i != size_of_the_chessboard - 1:
                string_to_be_printed += '\n'
        print(string_to_be_printed)
        print(f"That's the solution #{number_of_solutions}")
    else:
        for i in range(size_of_the_chessboard - 1, -1, -1):
            should_we_add_it = True
            if current_attempt.count(i):
                should_we_add_it = False
            diagonal_of_the_queen_we_are_trying_to_add = i + len(current_attempt)
            antidiagonal_of_the_queen_we_are_trying_to_add = i - len(current_attempt)
            for j in range(0, len(current_attempt)):
                if current_attempt[j] + j == diagonal_of_the_queen_we_are_trying_to_add:
                    should_we_add_it = False
                if current_attempt[j] - j == antidiagonal_of_the_queen_we_are_trying_to_add:
                    should_we_add_it = False
            if should_we_add_it:
                new_attempt = current_attempt.copy()
                new_attempt.append(i)
                stack.append(new_attempt)
print("Found soulutions:", number_of_solutions)
