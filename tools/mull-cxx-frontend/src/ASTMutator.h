#pragma once

#include <clang/AST/Expr.h>

class ASTMutation;
class BinaryMutator;
class RemoveVoidMutator;
class UnaryOperatorOpcodeReplacementMutator;
class UnaryOperatorRemovalMutator;
class ReplaceNumericAssignmentMutator;

class ASTMutator {
public:
  virtual ~ASTMutator() {}

  virtual void performBinaryMutation(ASTMutation &mutation, BinaryMutator &binaryMutator) = 0;
  virtual void performRemoveVoidMutation(ASTMutation &mutation,
                                         RemoveVoidMutator &removeVoidMutator) = 0;
  virtual void performUnaryOperatorOpcodeReplacementMutation(
      ASTMutation &mutation,
      UnaryOperatorOpcodeReplacementMutator &unaryOperatorOpcodeReplacementMutator) = 0;

  virtual void
  performUnaryOperatorRemovalMutation(ASTMutation &mutation,
                                      UnaryOperatorRemovalMutator &unaryNotToNoopMutator) = 0;
  virtual void performReplaceNumericAssignmentMutation(
      ASTMutation &mutation, ReplaceNumericAssignmentMutator &replaceNumericAssignmentMutator) = 0;
};

class Mutator {
public:
  virtual void performMutation(ASTMutation &mutation, ASTMutator &mutator) = 0;
  virtual ~Mutator() {}
};

class BinaryMutator : public Mutator {
public:
  clang::BinaryOperator::Opcode replacementOpCode;

  BinaryMutator(clang::BinaryOperator::Opcode replacementOpCode)
      : replacementOpCode(replacementOpCode) {}
  void performMutation(ASTMutation &mutation, ASTMutator &mutator) {
    mutator.performBinaryMutation(mutation, *this);
  }
  ~BinaryMutator() {}
};

class RemoveVoidMutator : public Mutator {
public:
  void performMutation(ASTMutation &mutation, ASTMutator &mutator) {
    mutator.performRemoveVoidMutation(mutation, *this);
  }
  ~RemoveVoidMutator() {}
};

class UnaryOperatorOpcodeReplacementMutator : public Mutator {
public:
  clang::UnaryOperator::Opcode replacementOpCode;

  UnaryOperatorOpcodeReplacementMutator(clang::UnaryOperator::Opcode replacementOpCode)
      : replacementOpCode(replacementOpCode) {}

  void performMutation(ASTMutation &mutation, ASTMutator &mutator) {
    mutator.performUnaryOperatorOpcodeReplacementMutation(mutation, *this);
  }

  ~UnaryOperatorOpcodeReplacementMutator() {}
};

class UnaryOperatorRemovalMutator : public Mutator {
public:
  clang::UnaryOperator *unaryOperator;

  UnaryOperatorRemovalMutator(clang::UnaryOperator *unaryOperator) : unaryOperator(unaryOperator) {}
  void performMutation(ASTMutation &mutation, ASTMutator &mutator) {
    mutator.performUnaryOperatorRemovalMutation(mutation, *this);
  }
  ~UnaryOperatorRemovalMutator() {}
};

class ReplaceNumericAssignmentMutator : public Mutator {

public:
  clang::BinaryOperator *assignmentBinaryOperator;

  ReplaceNumericAssignmentMutator(clang::BinaryOperator *assignmentBinaryOperator)
      : assignmentBinaryOperator(assignmentBinaryOperator) {}

  void performMutation(ASTMutation &mutation, ASTMutator &mutator) {
    mutator.performReplaceNumericAssignmentMutation(mutation, *this);
  }
};
