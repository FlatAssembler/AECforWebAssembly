import itertools
a = int(input("a="))
b = int(input("b="))
c = int(input("c="))
d = int(input("d="))
e = int(input("e="))
f = int(input("f="))
initialList = [a, b, c, d, e, f]
initialList.sort()
allPermutations = itertools.permutations(initialList)
maximum = 0
first = 0
second = 0
for permutation in allPermutations:
    currentFirst = permutation[0]*100 + permutation[1]*10 + permutation[2]
    currentSecond = permutation[3]*100 + permutation[4]*10 + permutation[5]
    currentSum = currentFirst + currentSecond
    if currentSum >= maximum and currentSum < 1000:
        maximum = currentSum
        first = currentFirst
        second = currentSecond
if maximum == 0 or second < 100:
    print("Bilbo")
else:
    print(f"{maximum}={first}+{second}")
