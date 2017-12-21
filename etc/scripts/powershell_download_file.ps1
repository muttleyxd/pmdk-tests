#
# Copyright 2017, Intel Corporation
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in
#       the documentation and/or other materials provided with the
#       distribution.
#
#     * Neither the name of the copyright holder nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

param (
  [string]$URL,
  [string]$PATH,
  [string]$SHA256HASH
)

# Create directory which will contain created file if it does not exist
$DIR_TO_CREATE = (Split-Path $PATH)

if (-Not (Test-Path "$DIR_TO_CREATE"))
{
	New-Item $DIR_TO_CREATE -ItemType directory -Force | Out-Null
}

# Checks if file is correct (exists and has correct SHA256 checksum)
Function Is-File-Correct
{
	if (-Not (Test-Path "$PATH"))
	{
		return $false
	}

	$HASH = (Get-FileHash "$PATH" -Algorithm SHA256 | Select-Object -ExpandProperty Hash)
	if ("$HASH" -eq "$SHA256HASH")
	{
		return $true
	}
	return $false
}

# If file is not correct then download it again
for ($i = 1; $i -le 10; $i++)
{
	if (Is-File-Correct)
	{
		exit 0
	}
	else
	{
		Write-Host "Downloading, try $i of 10"
		(New-Object Net.WebClient).DownloadFile("${URL}", "${PATH}")
	}
}
if (Is-File-Correct)
{
	exit 0
}
exit 1
