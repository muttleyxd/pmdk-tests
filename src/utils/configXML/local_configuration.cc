/*
 * Copyright 2017, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *
 *     * Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "local_configuration.h"

int LocalConfiguration::FillConfigFields(pugi::xml_node&& root) {
  root = root.child("localConfiguration");

  if (root.empty()) {
    std::cout << "Cannot find 'localConfiguration' node" << std::endl;
    return -1;
  }

  working_dir_ = root.child("testDir").text().get();

  if (working_dir_.empty() || !api_c_.DirectoryExists(this->working_dir_)) {
    PrintErrMsg("testDir");
    return -1;
  }

  if (api_c_.CreateDirectoryT(
          (working_dir_ + SEPARATOR + "pmdk_tests").c_str()) != 0) {
    return -1;
  }

  working_dir_ += SEPARATOR + "pmdk_tests" + SEPARATOR;

  return 0;
}

void LocalConfiguration::PrintErrMsg(const std::string& field) {
  std::cout << "Directory does not exist. Please change " << field << " field."
            << std::endl;
}

std::string LocalConfiguration::GetWorkingDir() {
  return this->working_dir_;
}
