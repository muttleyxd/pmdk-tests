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

#include "api_c/api_c.h"
#include "pmempool_dump.h"

/**
 * PMEMPOOL_DUMP_01
 * Dump data from log pool filled with a pattern
 * \test
 *          \li \c Step1. Create log pool with 40M size and fill pool with 'A'
 * character / SUCCESS
 *          \li \c Step2. Call 'pmempool dump' on log pool and verify exit state
 * / SUCCESS
 *          \li \c Step3. Check if output reflects actual pool state
 */
TEST_F(PmempoolDump, PMEMPOOL_DUMP_01) {
  /* Step 1 */
  errno = 0;
  ASSERT_EQ(0, CreateAndFillLogPool(pool_path_)) << pmemlog_errormsg();
  /* Step 2 */
  errno = 0;
  ASSERT_EQ(0, RunDump({}, pool_path_)) << pmemlog_errormsg();
  /* Step 3 */
  // TODO: implement DumpParser
}
#include <chrono>
using namespace std::chrono;

/**
 * PMEMPOOL_DUMP_02
 * Dump data from log pool, filled with a mixed pattern, both to standard output
 * and file.
 * \test
 *          \li \c Step1. Create log pool with 40M size and fill pool with
 * 'AAAA...BBBB' pattern / SUCCESS
 *          \li \c Step2. Call 'pmempool dump' on log pool / SUCCESS
 *          \li \c Step3. Call 'pmempool dump' on log pool and print output to
 * file / SUCCESS
 *          \li \c Step4. Check if outputs are the same
 */
TEST_F(PmempoolDump, PMEMPOOL_DUMP_02) {
  /* Step 1 */
  errno = 0;
  ASSERT_EQ(0, CreateAndFillLogPool(pool_path_, LogPatternType::AB))
      << pmemlog_errormsg();
  /* Step 2 */
  errno = 0;
  ASSERT_EQ(0, RunDump({}, pool_path_)) << pmemlog_errormsg();
  std::string output(GetOutputContent());
  /* Step 3 */
  errno = 0;
  ASSERT_EQ(
      0,
      RunDump({{Option::Output, OptionType::Short, test_dir_ + "output.txt"}},
              pool_path_))
      << pmemlog_errormsg();
  /* Step 4 */
  std::string content;
  ASSERT_EQ(0, api_c_.ReadFile(test_dir_ + "output.txt", content))
      << strerror(errno);
  ASSERT_EQ(output, content);
  // TODO: implement DumpParser
}

/**
 * PMEMPOOL_DUMP_03
 * Dump data from log pool, filled with a mixed pattern, in binary mode both to
 * standard output and file.
 * \test
 *          \li \c Step1. Create log pool with 40M size and fill pool with
 * 'AAAA...BBBB' pattern / SUCCESS
 *          \li \c Step2. Call 'pmempool dump' in binary mode on log pool /
 * SUCCESS
 *          \li \c Step3. Call 'pmempool dump' in binary on log pool and print
 * output to file / SUCCESS
 *          \li \c Step4. Check if outputs are the same
 */
TEST_F(PmempoolDump, PMEMPOOL_DUMP_03) {
  /* Step 1 */
  errno = 0;
  ASSERT_EQ(0, CreateAndFillLogPool(pool_path_)) << pmemlog_errormsg();
  /* Step 2 */
  errno = 0;
  ASSERT_EQ(0, RunDump({{Option::Binary, OptionType::Short, ""}}, pool_path_))
      << pmemlog_errormsg();
  std::string output(GetOutputContent());
  /* Step 3 */
  errno = 0;
  ASSERT_EQ(
      0,
      RunDump({{Option::Binary, OptionType::Short, ""},
               {Option::Output, OptionType::Short, test_dir_ + "output.txt"}},
              pool_path_))
      << pmemlog_errormsg();
  /* Step 4 */
  std::string content;
  ASSERT_EQ(0, api_c_.ReadFile(test_dir_ + "output.txt", content))
      << strerror(errno);
  ASSERT_EQ(output, content);
  // TODO: implement DumpParser
}

/**
 * PMEMPOOL_DUMP_04
 * Dump data from log pool, filled with a pattern, using range option.
 * \test
 *          \li \c Step1. Create log pool with 40M size and fill pool with 'A'
 * character / SUCCESS
 *          \li \c Step2. Call 'pmempool dump' with range option, selecting
 * given bytes / SUCCESS
 *          \li \c Step3. Call 'pmempool dump' with range option, selecting
 * range of bytes / SUCCESS
 */
TEST_F(PmempoolDump, PMEMPOOL_DUMP_04) {
  /* Step 1 */
  errno = 0;
  ASSERT_EQ(0, CreateAndFillLogPool(pool_path_)) << pmemlog_errormsg();
  /* Step 2 */
  errno = 0;
  ASSERT_EQ(
      0, RunDump({{Option::Range, OptionType::Short, "4,10,100"}}, pool_path_))
      << pmemlog_errormsg();
  errno = 0;
  ASSERT_EQ(0, RunDump({{Option::Range, OptionType::Short, "5,100,1000"}},
                       pool_path_))
      << pmemlog_errormsg();
  /* Step 3 */
  errno = 0;
  ASSERT_EQ(
      0, RunDump({{Option::Range, OptionType::Short, "1024-1030"}}, pool_path_))
      << pmemlog_errormsg();
  // TODO: implement DumpParser
}

/**
 * PMEMPOOL_DUMP_05
 * Dump data from log pool, filled with a pattern, using range option. Set chunk
 * size.
 */
TEST_F(PmempoolDump, PMEMPOOL_DUMP_05) {
  /* Step 1 */
  errno = 0;
  ASSERT_EQ(0, CreateAndFillLogPool(pool_path_)) << pmemlog_errormsg();
  /* Step 2 */
  errno = 0;
  ASSERT_EQ(0,
            RunDump({{Option::Range, OptionType::Short, "0-100"}}, pool_path_))
      << pmemlog_errormsg();
  errno = 0;
  ASSERT_EQ(0, RunDump({{Option::Range, OptionType::Short,
                         "100000000000000000-100000000000000001"}},
                       pool_path_))
      << pmemlog_errormsg();
  /* Step 3 */
  errno = 0;
  ASSERT_EQ(0, RunDump({{Option::Range, OptionType::Short, "1000-1001"},
                        {Option::Chunk, OptionType::Short, "4"}},
                       pool_path_))
      << pmemlog_errormsg();
  // TODO: implement DumpParser
}

/**
 * PMEMPOOL_DUMP_06
 * Dump data from log pool, filled with a pattern, using range bytes option.
 */
TEST_F(PmempoolDump, PMEMPOOL_DUMP_06) {
  /* Step 1 */
  errno = 0;
  ASSERT_EQ(0, CreateAndFillLogPool(pool_path_)) << pmemlog_errormsg();
  /* Step 2 */
  errno = 0;
  ASSERT_EQ(
      0, RunDump({{Option::Range, OptionType::Short, "32B-64B"}}, pool_path_))
      << pmemlog_errormsg();
  // TODO: implement DumpParser
}

/**
 * PMEMPOOL_DUMP_07
 * Dump data from log pool described with poolset file consisting of two parts,
 * filled with a pattern, using range bytes option.
 */
TEST_F(PmempoolDump, PMEMPOOL_DUMP_07) {
  /* Step 1 */
  errno = 0;
  ASSERT_EQ(0, CreateAndFillLogPool(
                   pool_path_, A, "PMEMPOOLSET\n20M " + pool_path_ +
                                      ".part1\n20M " + pool_path_ + ".part2\n"))
      << pmemlog_errormsg();
  /* Step 2 */
  errno = 0;
  ASSERT_EQ(0, RunDump({}, pool_path_)) << pmemlog_errormsg();
  // TODO: implement DumpParser
}

/**
 * PMEMPOOL_DUMP_08
 * Dump data from blk pool
 */
TEST_F(PmempoolDump, PMEMPOOL_DUMP_08) {
  /* Step 1 */
  errno = 0;
  ASSERT_EQ(0, CreateAndFillBlkPool(pool_path_)) << pmemlog_errormsg();
  /* Step 2 */
  errno = 0;
  ASSERT_EQ(0, RunDump({}, pool_path_)) << pmemlog_errormsg();
  // TODO: implement DumpParser
}

/**
 * PMEMPOOL_DUMP_09
 * Dump data from blk pool with pattern written to first block
 */
TEST_F(PmempoolDump, PMEMPOOL_DUMP_09) {
  /* Step 1 */
  errno = 0;
  ASSERT_EQ(0, CreateAndFillBlkPool(pool_path_, BlkPatternType::OneBlock))
      << pmemlog_errormsg();
  /* Step 2 */
  errno = 0;
  ASSERT_EQ(0, RunDump({}, pool_path_)) << pmemlog_errormsg();
  // TODO: implement DumpParser
}

/**
 * PMEMPOOL_DUMP_10
 * Dump data from blk pool
 */
TEST_F(PmempoolDump, PMEMPOOL_DUMP_10) {
  /* Step 1 */
  errno = 0;
  ASSERT_EQ(0, CreateAndFillBlkPool(pool_path_)) << pmemlog_errormsg();
  /* Step 2 */
  errno = 0;
  ASSERT_EQ(0,
            RunDump({{Option::Range, OptionType::Short, "0-100"}}, pool_path_))
      << pmemlog_errormsg();
  // TODO: implement DumpParser
}

/**
 * PMEMPOOL_DUMP_11
 * Dump data from blk pool with pattern written to first block, set chunk size
 */
TEST_F(PmempoolDump, PMEMPOOL_DUMP_11) {
  /* Step 1 */
  errno = 0;
  ASSERT_EQ(0, CreateAndFillBlkPool(pool_path_, BlkPatternType::OneBlock))
      << pmemlog_errormsg();
  /* Step 2 */
  errno = 0;
  ASSERT_EQ(
      1, RunDump(DumpArgs{{{Option::Chunk, OptionType::Short, "4"}},
                          "error: '--chunk|-c' -- invalid option specified for "
                          "pool type 'blk'\n"},
                 pool_path_))
      << pmemlog_errormsg();
  // TODO: implement DumpParser
}

/**
 * PMEMPOOL_DUMP_12
 * Print help message for pmempool dump
 */
TEST_F(PmempoolDump, PMEMPOOL_DUMP_12) {
  /* Step 1 */
  errno = 0;
  ASSERT_EQ(0, RunDump({{Option::Help, OptionType::Short, ""}}, ""))
      << pmemlog_errormsg();
  // TODO: implement DumpParser
}

/**
 * PMEMPOOL_DUMP_13
 * Invalid path
 */
TEST_F(PmempoolDump, PMEMPOOL_DUMP_13) {
  /* Step 1 */
  errno = 0;
  ASSERT_EQ(0, CreateAndFillLogPool(pool_path_)) << pmemlog_errormsg();
  /* Step 2 */
  errno = 0;
  ASSERT_EQ(1, RunDump(DumpArgs{{},
                                "error: read: Is a directory\nerror: " +
                                    test_dir_ + ": "
                                                "unknown pool type --"},
                       test_dir_))
      << pmemlog_errormsg();
  errno = 0;
  ASSERT_EQ(1,
            RunDump(DumpArgs{{}, "Usage: pmempool dump [<args>] <file>"}, ""))
      << pmemlog_errormsg();
  // TODO: implement DumpParser
}

/**
 * PMEMPOOL_DUMP_14
 * Dump data from blk pool with pattern written to first block, set chunk size
 */
TEST_F(PmempoolDump, PMEMPOOL_DUMP_14) {
  /* Step 1 */
  PMEMobjpool *pop =
      pmemobj_create(pool_path_.c_str(), nullptr, 40 * 1024 * 1024, 0666);
  ASSERT_NE(nullptr, pop);
  pmemobj_close(pop);
  /* Step 2 */
  errno = 0;
  ASSERT_EQ(1, RunDump(DumpArgs{{},
                                "error: " + pool_path_ +
                                    ": PMEMOBJ pool not supported\n"},
                       pool_path_))
      << pmemlog_errormsg();
  // TODO: implement DumpParser
}

/**
 * PMEMPOOL_DUMP_15
 * Dump data from blk pool with pattern written to first block, set chunk size
 */
TEST_F(PmempoolDump, PMEMPOOL_DUMP_15) {
  /* Step 1 */
  errno = 0;
  ASSERT_EQ(0, CreateAndFillLogPool(pool_path_)) << pmemlog_errormsg();
  /* Step 2 */
  errno = 0;
  ASSERT_EQ(0,
            RunDump({{Option::Range, OptionType::Short, "0-10"}}, pool_path_))
      << pmemlog_errormsg();

  errno = 0;
  ASSERT_EQ(0, RunDump({{Option::Range, OptionType::Short, "0-10"},
                        {Option::Binary, OptionType::Short, ""}},
                       pool_path_))
      << pmemlog_errormsg();

  errno = 0;
  ASSERT_EQ(0,
            RunDump({{Option::Range, OptionType::Short, "1000-"}}, pool_path_))
      << pmemlog_errormsg();

  errno = 0;
  ASSERT_EQ(0, RunDump({{Option::Range, OptionType::Short, "1000-"},
                        {Option::Binary, OptionType::Short, ""}},
                       pool_path_))
      << pmemlog_errormsg();

  errno = 0;
  ASSERT_EQ(0,
            RunDump({{Option::Range, OptionType::Short, "-1000"}}, pool_path_))
      << pmemlog_errormsg();

  errno = 0;
  ASSERT_EQ(0, RunDump({{Option::Range, OptionType::Short, "-1000"},
                        {Option::Binary, OptionType::Short, ""}},
                       pool_path_))
      << pmemlog_errormsg();
}

/**
 * PMEMPOOL_DUMP_16
 * Invalid chunk size
 */
TEST_F(PmempoolDump, PMEMPOOL_DUMP_16) {
  /* Step 1 */
  errno = 0;
  ASSERT_EQ(0, CreateAndFillLogPool(pool_path_)) << pmemlog_errormsg();
  /* Step 2 */
  errno = 0;
  ASSERT_EQ(1, RunDump(DumpArgs{{{Option::Chunk, OptionType::Short, "C"}},
                                "error: invalid chunk size specified 'C'"},
                       pool_path_))
      << pmemlog_errormsg();

  errno = 0;
  ASSERT_EQ(1, RunDump(DumpArgs{{{Option::Chunk, OptionType::Short, "-"}},
                                "error: invalid chunk size specified '-'"},
                       pool_path_))
      << pmemlog_errormsg();

  errno = 0;
  ASSERT_EQ(1, RunDump(DumpArgs{{{Option::Chunk, OptionType::Short, "-2"}},
                                "error: invalid chunk size specified '-2'"},
                       pool_path_))
      << pmemlog_errormsg();
  // TODO: implement DumpParser
}

/**
 * PMEMPOOL_DUMP_17
 * Invalid output file
 */
TEST_F(PmempoolDump, PMEMPOOL_DUMP_17) {
  /* Step 1 */
  errno = 0;
  ASSERT_EQ(0, CreateAndFillLogPool(pool_path_)) << pmemlog_errormsg();
  /* Step 2 */
  errno = 0;
  ASSERT_EQ(1, RunDump(DumpArgs{{},
                                "error: read: Is a directory\nerror: " +
                                    test_dir_ + ": unknown pool type --"},
                       test_dir_))
      << pmemlog_errormsg();
}
TEST_F(PmempoolDump, PMEMPOOL_DUMP_17_2) {
  /* Step 1 */
  errno = 0;
  ASSERT_EQ(0, CreateAndFillBlkPool(pool_path_)) << pmemlog_errormsg();
  /* Step 2 */
  errno = 0;
  ASSERT_EQ(1, RunDump(DumpArgs{{},
                                "error: read: Is a directory\nerror: " +
                                    test_dir_ + ": unknown pool type --"},
                       test_dir_))
      << pmemlog_errormsg();
}

/**
 * PMEMPOOL_DUMP_18
 * Invalid range
 */
TEST_F(PmempoolDump, PMEMPOOL_DUMP_18) {
  /* Step 1 */
  errno = 0;
  ASSERT_EQ(0, CreateAndFillLogPool(pool_path_)) << pmemlog_errormsg();
  /* Step 2 */
  errno = 0;
  ASSERT_EQ(1, RunDump(DumpArgs{{{Option::Range, OptionType::Short, "R"}},
                                "error: invalid range value specified -- "
                                "'R'\nerror: " +
                                    pool_path_ + ": "
                                                 "dumping pool file failed"},
                       pool_path_))
      << pmemlog_errormsg();
  // TODO: implement DumpParser
}

/**
 * PMEMPOOL_DUMP_19
 * Empty blk and log pool
 */
TEST_F(PmempoolDump, PMEMPOOL_DUMP_19) {
  /* Step 1 */
  errno = 0;
  ASSERT_EQ(0, CreateAndFillLogPool(pool_path_, LogPatternType::Empty))
      << pmemlog_errormsg();
  /* Step 2 */
  errno = 0;
  ASSERT_EQ(0, RunDump(DumpArgs{}, pool_path_)) << pmemlog_errormsg();
}
TEST_F(PmempoolDump, PMEMPOOL_DUMP_19_2) {
  /* Step 1 */
  errno = 0;
  ASSERT_EQ(0, CreateAndFillBlkPool(pool_path_, BlkPatternType::EmptyPool))
      << pmemlog_errormsg();
  /* Step 2 */
  errno = 0;
  ASSERT_EQ(0, RunDump(DumpArgs{}, pool_path_)) << pmemlog_errormsg();
}

/*********************
 * TODO: PORT TESTS 20 and 21
 * reduce number of tests
 * port DumpParser
 * *******************
 */
