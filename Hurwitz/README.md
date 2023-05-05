# Implementation of the Hurwitz Criterion in AEC

This is my implementation of the [Hurwitz Criterion](https://en.wikipedia.org/wiki/Routh%E2%80%93Hurwitz_stability_criterion) in AEC. You can (hopefully) see it alive [on my website](https://flatassembler.github.io/hurwitz.html).

Right now, it can solve the general case of the Hurwitz Criterion, and it can solve the first special case of the Hurwitz criterion, namely, when there is a zero in the first column of the matrix, but there is no row of zeros. It cannot yet solve the second special case of the Hurwitz criterion, when there is a row entirely of zeros (in which case, the system is either marginally stable or unstable, it cannot be stable).