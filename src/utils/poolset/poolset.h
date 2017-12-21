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

#ifndef PMDK_TESTS_SRC_UTILS_POOLSET_POOLSET_H_
#define PMDK_TESTS_SRC_UTILS_POOLSET_POOLSET_H_

#include <memory>
#include "configXML/local_configuration.h"
#include "replica.h"

extern std::unique_ptr<LocalConfiguration> local_config;
using replica = std::initializer_list<std::string>;

class Poolset final {
 private:
  int replica_counter_ = 0;
  std::string path_ = local_config->GetTestDir();
  std::string name_ = "pool.set";
  std::string full_path_ = local_config->GetTestDir() + SEPARATOR + name_;
  std::vector<Replica> replicas_;
  void InitializeReplicas(std::initializer_list<replica> &&content);

 public:
  Poolset() = default;
  Poolset(std::initializer_list<replica> content) {
    InitializeReplicas(std::move(content));
  };
  Poolset(const std::string &path, std::initializer_list<replica> content)
      : path_(path) {
    InitializeReplicas(std::move(content));
  };
  Poolset(const std::string &path, const std::string &name,
          std::initializer_list<replica> content)
      : path_(path), name_(name) {
    full_path_ = path + SEPARATOR + name;
    InitializeReplicas(std::move(content));
  };

  const std::string &GetName() const {
    return this->name_;
  };
  const std::string &GetFullPath() const {
    return full_path_;
  }
  const Replica &GetReplica(unsigned index) const {
    return replicas_.at(index);
  }
  const std::vector<Replica> &GetReplicas() const {
    return this->replicas_;
  };
  std::vector<Part> GetParts() const;
  std::vector<std::string> GetContent() const;
};

#endif  // !PMDK_TESTS_SRC_UTILS_POOLSET_POOLSET_H_
