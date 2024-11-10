import pathlib
import hashlib
import io
import hmac

FilesForConcatenation = ".\\FilesForConcatenation.txt"
Lines = []
OutputCodeLines = []
PreprocessorLines = []

with open(FilesForConcatenation, "r") as f:
    Lines = f.readlines()
    f.close()

for File in Lines:
    File = File.strip()
    if len(File) == 0:
        continue
    with open(File, "r") as f:
        FileContentLines = f.readlines()
        for SrcLine in FileContentLines:
            SrcLine = SrcLine.strip("\n")
            if SrcLine.find("#include") == -1 and SrcLine.find("#pragma") == -1:
                OutputCodeLines.append(SrcLine)
            elif SrcLine.find("#include <") >= 0:
                if SrcLine not in PreprocessorLines:
                    PreprocessorLines.append(SrcLine)
        f.close()


for SrcLine in PreprocessorLines:
    print(SrcLine)

for SrcLine in OutputCodeLines:
    print(SrcLine)
