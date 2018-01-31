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

#ifndef PMDK_TESTS_SRC_TESTS_PMEMPOOLS_PMEMPOOL_DUMP_PMEMPOOL_DUMP_H_
#define PMDK_TESTS_SRC_TESTS_PMEMPOOLS_PMEMPOOL_DUMP_PMEMPOOL_DUMP_H_

#include "configXML/local_configuration.h"
#include "gtest/gtest.h"
#include "output/output.h"
#include "shell/i_shell.h"
#include "string_utils.h"
#include "structures.h"
#include "test_utils/file_utils.h"

#include <memory>

extern std::unique_ptr<LocalConfiguration> local_config;

// enum class PoolType { Obj, Blk, Log, None, Count };

class PmempoolDump : public ::testing::Test {
 private:
  std::string err_msg_;
  Output<> output_;

 public:
  ApiC api_c_;
  IShell shell_;
  PoolsetManagement p_mgmt_;
  const std::string test_dir_ = local_config->GetTestDir();
  const std::string pool_path_ = local_config->GetTestDir() + "pool.file";
  const std::string inherit_file_path_ =
      local_config->GetTestDir() + "inherited.pool";

  const std::string &GetOutputContent() const {
    return output_.GetContent();
  }

  int CallDump(const std::string &path) {
    output_ = shell_.ExecuteCommand("pmempool dump " + path);
    return output_.GetExitCode();
  }

  enum LogPatternType { A, AB, Empty };
  enum BlkPatternType { Fill, OneBlock, EmptyPool };

  int CreateAndFillLogPool(const std::string &path,
                           const LogPatternType &type = A,
                           const std::string &poolset = "") {
    int pool_size = 40 * 1024 * 1024;
    if (poolset != "") {
      if (api_c_.CreateFileT(path, poolset) != 0)
        return 1;
      pool_size = 0;
    }
    PMEMlogpool *plp = pmemlog_create(path.c_str(), pool_size, 0666);
    if (!plp)
      return 1;

    if (type != LogPatternType::Empty) {
      size_t usable_size = pmemlog_nbyte(plp);
      if (usable_size == 0)
        return 1;

      char *buffer = new char[usable_size];
      memset(buffer, 'A', usable_size);

      if (type == AB)
        memset(buffer + usable_size / 2, 'B', usable_size / 2);

      if (pmemlog_append(plp, buffer, usable_size) != 0)
        return 1;
    }

    pmemlog_close(plp);
    return 0;
  }

  int CreateAndFillBlkPool(const std::string &path,
                           const BlkPatternType &type = Fill,
                           const std::string &poolset = "") {
    int pool_size = 40 * 1024 * 1024;
    if (poolset != "") {
      if (api_c_.CreateFileT(path, poolset) != 0)
        return 1;
      pool_size = 0;
    }
    PMEMblkpool *pbp =
        pmemblk_create(path.c_str(), PMEMBLK_MIN_BLK, pool_size, 0666);
    if (!pbp)
      return 1;

    size_t blk_count = pmemblk_nblock(pbp);
    if (blk_count < 1)
      return 1;

    if (type == Fill)
      blk_count = 1;
    else if (type == BlkPatternType::EmptyPool)
      blk_count = 0;

    char buffer[PMEMBLK_MIN_BLK];
    char read_buffer[PMEMBLK_MIN_BLK];

    for (size_t i = 0; i < blk_count; ++i) {
      if (pmemblk_write(pbp, buffer, i) != 0 ||
          pmemblk_read(pbp, read_buffer, i) != 0 ||
          memcmp(buffer, read_buffer, PMEMBLK_MIN_BLK) != 0) {
        return 1;
      }
    }

    pmemblk_close(pbp);
    return 0;
  }

  int RunDump(const DumpArgs &dump_args, const std::string &path) {
    std::string dump_args_str =
        struct_utils::CombineArguments(dump_args.args) + " " + path;

    output_ = shell_.ExecuteCommand("pmempool dump " + dump_args_str);

    if (dump_args.err_msg != "" &&
        !string_utils::IsSubstrFound(dump_args.err_msg, output_.GetContent())) {
      EXPECT_EQ(dump_args.err_msg, output_.GetContent());
      return 1;
    }

    return output_.GetExitCode();
  }

  virtual void TearDown();
};

#endif  // !PMDK_TESTS_SRC_TESTS_PMEMPOOLS_PMEMPOOL_DUMP_PMEMPOOL_DUMP_H_
