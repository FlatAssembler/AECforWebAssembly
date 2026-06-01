# Copilot instructions — AECforWebAssembly

Purpose: quick reference for Copilot sessions working on this repo.

## Build, test and (format) commands
- Quick native build (minimal):
  - g++ -o aec AECforWebAssembly.cpp -std=c++11 -O3
  - clang++ -o aec AECforWebAssembly.cpp -O3
- CMake (recommended / CI):
  - cmake -B build -DCMAKE_BUILD_TYPE=Release
  - cmake --build build --config Release
  - ctest -C Release --rerun-failed --output-on-failure
- Run a single CTest by name:
  - ctest -R <test_name> -C Release --output-on-failure
    (test names are defined in CMakeLists.txt, e.g. structureDeclarationTestRuns)
- Direct single-test/manual steps:
  - Compile a single AEC program: ./aec <path/to/test>.aec
  - Assemble produced WAT to WASM: npx -q -p wabt wat2wasm <file>.wat
  - Run NodeJS test: node <test>.js
- Emscripten / npm targets (produce wasm/js artifacts):
  - npm run install  (runs `emcmake cmake .` then `emmake make && emmake make install`)
  - npm test (maps to `emmake make test`)
  - Node engine required: node >= 11 (package.json).

## High-level architecture
- The project is a compiler written in C++ (entry files: AECforWebAssembly.cpp plus parser/tokenizer/semantic modules). It translates AEC source (.aec) to WebAssembly text (.wat).
- Conversion pipeline used by tests/CI: aec (C++ compiler) -> wat2wasm (wabt) -> node runner executes generated WASM.
- CMake orchestrates builds and test cases. Tests typically run three steps: compile .aec, assemble .wat to .wasm, and execute via node (when applicable).
- package.json contains emscripten-oriented build scripts (emcmake/emmake) to build a WebAssembly-targeted artifact (used for wasm/js releases).

## Key repository conventions
- Tests are defined in CMakeLists.txt with explicit names (use those names with ctest -R). Many tests live at repository root (e.g. leftHandSideConditionalTest.aec) or inside example subdirectories (e.g. HuffmanCodingInAEC/).
- Node-based test runners are common and usually named `automatedTest.js` or `<example>.js` inside each example folder.
- Use `npx -q -p wabt wat2wasm` for deterministic wat2wasm invocations (this is used in CMake and CI).
- AEC language notes (project-specific syntax): variables are typed (Character, Integer16, Decimal64, plus Pointer variants); semicolons terminate statements; control flow tokens use `Then` and `Loop`; boolean operators are `and` / `or` (not `&`/`|`). These semantics appear across tests and examples.
- Formatting: there is no repo-wide linter configured. Recommended: clang-format for C++ and Prettier for JS (not enforced by CI).
- CI/analysis files to reference:
  - GitHub Actions: .github/workflows/cmake.yml (build + ctest), .github/workflows/build.yml (SonarCloud + build-wrapper)
  - GitLab CI: .gitlab-ci.yml (cmake + ctest)
  - sonar-project.properties present for SonarCloud settings.

## Quick pointers for editing or adding tests
- To add a test that compiles and runs: add an `add_test(NAME ...)` entry in CMakeLists.txt mirroring the existing 3-step pattern (compile -> wat2wasm -> node). Use consistent naming so maintainers can run it with `ctest -R`.
- When adding Node-based checks, match existing examples' usage of `npx -q -p wabt wat2wasm` to avoid requiring global wabt installation.

## Other AI/assistant configs
- No CLAUDE.md, AGENTS.md, .cursorrules, AIDER_CONVENTIONS.md, or other AI-assistant rule files were found.

---
Reference: README.md, CMakeLists.txt, package.json, .github/workflows/*, .gitlab-ci.yml, sonar-project.properties
