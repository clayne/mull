extern "C" {
extern int printf(const char *, ...);
}

bool negation(bool arg) {
  return !arg;
}

int main() {
  if (negation(true) == false) {
    printf("NORMAL\n");
    return 0;
  } else {
    printf("MUTATED\n");
    return 1;
  }
}

// clang-format off

/**
RUN: %clang_cxx %sysroot -fplugin=%mull_frontend_cxx %s -o %s.exe

RUN: %s.exe | %filecheck %s --dump-input=fail --strict-whitespace --match-full-lines --check-prefix=STANDALONE_WITHOUT_MUTATION
RUN: (env "cxx_remove_negation:%s:6:10:6:11"=1 %s.exe || true) | %filecheck %s --dump-input=fail --strict-whitespace --match-full-lines --check-prefix=STANDALONE_WITH_MUTATION

STANDALONE_WITHOUT_MUTATION:NORMAL
STANDALONE_WITH_MUTATION:MUTATED

RUN: %mull_runner %s.exe -ide-reporter-show-killed | %filecheck %s --dump-input=fail --strict-whitespace --match-full-lines --check-prefix=MULL_RUNNER

MULL_RUNNER:[info] Killed mutants (1/1):
MULL_RUNNER:{{.*}}sample.cpp:6:10: warning: Killed: Replaced !a with a [cxx_remove_negation]
*/
