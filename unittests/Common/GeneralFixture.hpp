#pragma once

#include "Nest/Api/Node.h"
#include "Nest/Api/Backend.h"

//! General fixture to be used for unit testing.
//!
//! This ensures proper initialization and shutdown of Sparrow code. It ensures that we can restart
//! the compiler state after a test is run.
//!
//! It also provide some generic utilities for testing.
struct GeneralFixture {
    GeneralFixture();
    ~GeneralFixture();

    static GeneralFixture& instance();

    //! Create a new location; each time we increment the line number
    Location createLocation();

    //! Create a datatype with the given name
    Node* createDatatypeNode(StringRef name, CompilationContext* ctx = nullptr);
    //! Create a datatype with the given name, and with the same native name
    Node* createNativeDatatypeNode(StringRef name, CompilationContext* ctx = nullptr);

    //! Create a simple concept with no if-clause
    Node* createSimpleConcept(StringRef name, CompilationContext* ctx = nullptr);

    //! The global context we are using
    CompilationContext* globalContext_{nullptr};

    //! The backend mock we are using
    unique_ptr<Backend> backend_{};

private:
    //! The last instance of this class
    static GeneralFixture* lastInstance_;
    //! The source code we are using for all generated code
    SourceCode* sourceCode_{nullptr};
    //! The last line number we've used for locations
    int lastLineNum_{0};
};
