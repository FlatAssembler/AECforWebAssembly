void main() {
  var NDEBUG = true;
  List<List<int>> myStack = [[]];
  int n = 8, counterOfSolutions = 0;
  while (myStack.length > 0) {
    var currentChessboard = myStack[myStack.length - 1];
    myStack.removeAt(myStack.length - 1);
    if (!NDEBUG) {
      print("The current chessboard has ${currentChessboard.length} queens:");
      for (var queen in currentChessboard) print(queen);
    }
    if (currentChessboard.length == n) {
      counterOfSolutions++;
      var solution = StringBuffer("");
      for (var i = 0; i < n; i++) {
        solution.write(String.fromCharCode('A'.codeUnitAt(0) + i));
        solution.write(currentChessboard[i] + 1);
        solution.write(" ");
      }
      print(solution.toString());
      var printedChessboard = StringBuffer("  +");
      for (var i = 0; i < n; i++) printedChessboard.write("-+");
      printedChessboard.write("\n");
      for (var row = n - 1; row >= 0; row--) {
        if (row + 1 < 10)
          printedChessboard.write(" ${row + 1}");
        else
          printedChessboard.write("${row + 1}");
        printedChessboard.write("|");
        for (var column = 0; column < n; column++)
          printedChessboard.write(
              "${currentChessboard[column] == row ? 'Q' : (row + column) % 2 == 0 ? '*' : ' '}|");
        printedChessboard.write("\n  +");
        for (var i = 0; i < n; i++) printedChessboard.write("-+");
        printedChessboard.write("\n");
      }
      printedChessboard.write("   ");
      for (var column = 0; column < n; column++)
        printedChessboard
            .write("${String.fromCharCode('A'.codeUnitAt(0) + column)} ");
      print(printedChessboard.toString());
    } else {
      var rowOfTheQueenWeAreAdding = currentChessboard.length;
      for (var columnOfTheQueenWeAreAdding = n - 1;
          columnOfTheQueenWeAreAdding >= 0;
          columnOfTheQueenWeAreAdding--) {
        if (!NDEBUG) {
          print(
              "Attempting to add a queen at the column $columnOfTheQueenWeAreAdding.");
        }
        var isThereAQueenInTheSameColumn = false;
        for (var i = 0; i < currentChessboard.length; i++)
          if (currentChessboard[i] == columnOfTheQueenWeAreAdding) {
            isThereAQueenInTheSameColumn = true;
          }
        if (isThereAQueenInTheSameColumn && !NDEBUG) {
          print("There is a queen in the same column!");
        }
        var isThereAQueenInTheSameDiagonal = false;
        for (var i = 0; i < currentChessboard.length; i++) {
          if (currentChessboard[i] + i ==
              columnOfTheQueenWeAreAdding + rowOfTheQueenWeAreAdding)
            isThereAQueenInTheSameDiagonal = true;
          if (currentChessboard[i] - i ==
              columnOfTheQueenWeAreAdding - rowOfTheQueenWeAreAdding)
            isThereAQueenInTheSameDiagonal = true;
        }
        if (isThereAQueenInTheSameDiagonal && !NDEBUG) {
          print("There is a queen in the same diagonal!");
        }
        if (!isThereAQueenInTheSameColumn && !isThereAQueenInTheSameDiagonal) {
          var newChessboard = List<int>.from(
              currentChessboard); // That's how you copy an array in Dart. Dart is not like C++ in that regard.
          newChessboard.add(columnOfTheQueenWeAreAdding);
          myStack.add(newChessboard);
        }
      }
    }
  }
  print("Found $counterOfSolutions solutions!");
}
