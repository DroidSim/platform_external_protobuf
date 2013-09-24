// Protocol Buffers - Google's data interchange format
// Copyright 2008 Google Inc.  All rights reserved.
// http://code.google.com/p/protobuf/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Author: kenton@google.com (Kenton Varda)
//  Based on original Protocol Buffers design by
//  Sanjay Ghemawat, Jeff Dean, and others.

#include <map>
#include <string>

#include <google/protobuf/compiler/javanano/javanano_params.h>
#include <google/protobuf/compiler/javanano/javanano_enum.h>
#include <google/protobuf/compiler/javanano/javanano_helpers.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/stubs/strutil.h>

namespace google {
namespace protobuf {
namespace compiler {
namespace javanano {

EnumGenerator::EnumGenerator(const EnumDescriptor* descriptor, const Params& params)
  : params_(params), descriptor_(descriptor) {
  for (int i = 0; i < descriptor_->value_count(); i++) {
    const EnumValueDescriptor* value = descriptor_->value(i);
    const EnumValueDescriptor* canonical_value =
      descriptor_->FindValueByNumber(value->number());

    if (value == canonical_value) {
      canonical_values_.push_back(value);
    } else {
      Alias alias;
      alias.value = value;
      alias.canonical_value = canonical_value;
      aliases_.push_back(alias);
    }
  }
}

EnumGenerator::~EnumGenerator() {}

void EnumGenerator::Generate(io::Printer* printer) {
  printer->Print(
      "// enum $classname$\n",
      "classname", descriptor_->name());

  // Start of shell class
  bool use_shell_class = params_.java_enum_style();
  if (use_shell_class) {
    bool is_own_file = descriptor_->containing_type() == NULL
        && params_.java_multiple_files(descriptor_->file()->name());
    printer->Print(
      "public $modifiers$final class $classname$ {\n"
      "  private $classname$() {}\n"
      "  \n",
      "modifiers", is_own_file ? "" : "static ",
      "classname", RenameJavaKeywords(descriptor_->name()));
    printer->Indent();
  }

  // Canonical values
  for (int i = 0; i < canonical_values_.size(); i++) {
    printer->Print(
      "public static final int $name$ = $canonical_value$;\n",
      "name", RenameJavaKeywords(canonical_values_[i]->name()),
      "canonical_value", SimpleItoa(canonical_values_[i]->number()));
  }

  // Aliases
  for (int i = 0; i < aliases_.size(); i++) {
    printer->Print(
      "public static final int $name$ = $canonical_name$;\n",
      "name", RenameJavaKeywords(aliases_[i].value->name()),
      "canonical_name", RenameJavaKeywords(aliases_[i].canonical_value->name()));
  }

  // End of shell class
  if (use_shell_class) {
    printer->Outdent();
    printer->Print("}\n");
  }
  printer->Print("\n");
}

}  // namespace javanano
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
