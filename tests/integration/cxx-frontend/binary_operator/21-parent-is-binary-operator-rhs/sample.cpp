extern "C" {
extern int printf(const char *, ...);
}
int x = 1;
int sum_times() {
  return (x * (x + x));
}

int main() {
  if (sum_times() == 2) {
    printf("NORMAL\n");
    return 0;
  } else {
    printf("MUTATED\n");
    return 1;
  }
}

// clang-format off

/**
RUN: %clang_cxx %sysroot -fplugin=%mull_frontend_cxx %s -o %s-ast.exe
RUN: %clang_cxx %sysroot -g %pass_mull_ir_frontend %s -o %s-ir.exe

RUN: %s-ast.exe | %filecheck %s --dump-input=fail --strict-whitespace --match-full-lines --check-prefix=STANDALONE_WITHOUT_MUTATION
RUN: (env "cxx_add_to_sub:%s:6:18:6:19"=1 %s-ast.exe || true) | %filecheck %s --dump-input=fail --strict-whitespace --match-full-lines --check-prefix=STANDALONE_WITH_MUTATION

RUN: %s-ir.exe | %filecheck %s --dump-input=fail --strict-whitespace --match-full-lines --check-prefix=STANDALONE_WITHOUT_MUTATION
RUN: (env "cxx_add_to_sub:%s:6:18:6:19"=1 %s-ir.exe || true) | %filecheck %s --dump-input=fail --strict-whitespace --match-full-lines --check-prefix=STANDALONE_WITH_MUTATION

STANDALONE_WITHOUT_MUTATION:NORMAL
STANDALONE_WITH_MUTATION:MUTATED

RUN: %mull_runner %s-ast.exe -ide-reporter-show-killed | %filecheck %s --dump-input=fail --strict-whitespace --match-full-lines --check-prefix=MULL_RUNNER
RUN: %mull_runner %s-ir.exe -ide-reporter-show-killed | %filecheck %s --dump-input=fail --strict-whitespace --match-full-lines --check-prefix=MULL_RUNNER

MULL_RUNNER:[info] Killed mutants (1/1):
MULL_RUNNER:{{.*}}sample.cpp:6:18: warning: Killed: Replaced + with - [cxx_add_to_sub]
*/
