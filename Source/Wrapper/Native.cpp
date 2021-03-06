/*
 * ShaderConductor
 *
 * Copyright (c) Microsoft Corporation. All rights reserved.
 * Licensed under the MIT License.
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons
 * to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED *AS IS*, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "Native.h"
#include <ShaderConductor/ShaderConductor.hpp>
#include <iostream>

using namespace ShaderConductor;

char* CopyString(const char* source)
{
    size_t sourceLength = strlen(source);
    
    msgArray = new char[sourceLength + 1];
    strncpy_s(msgArray, sourceLength + 1, source, sourceLength);

    return msgArray;
}

void Compile(SourceDescription* source, TargetDescription* target, ResultDescription* result)
{
    Compiler::SourceDesc sourceDesc;
    sourceDesc.entryPoint = source->entryPoint;
    sourceDesc.source = source->source;
    sourceDesc.stage = source->stage;

    Compiler::TargetDesc targetDesc;
    targetDesc.language = target->shadingLanguage;
    targetDesc.version = target->version;

    try
    {
        const auto translation = Compiler::Compile(std::move(sourceDesc), std::move(targetDesc));

        if (!translation.errorWarningMsg.empty())
        {
            const char* errorData = translation.errorWarningMsg.c_str();
            result->errorWarningMsg = CopyString(errorData);
        }
        if (!translation.target.empty())
        {
            result->targetSize = translation.target.size();
            binaryArray = new char[result->targetSize];
            std::copy(translation.target.data(), translation.target.data() + result->targetSize, binaryArray);

            result->target = binaryArray;
        }

        result->hasError = translation.hasError;
        result->isText = translation.isText;
    }
    catch (std::exception& ex)
    {
        const char* exception = ex.what();
        result->errorWarningMsg = CopyString(exception);
        result->hasError = true;
    }
}

void Disassemble(DisassembleDescription* source, ResultDescription* result)
{
    Compiler::DisassembleDesc disasembleSource;
    disasembleSource.language = source->language;
    disasembleSource.binary = std::vector<uint8_t>(source->binary, source->binary + source->binarySize);

    const auto disassembleResult = Compiler::Disassemble(disasembleSource);

    if (!disassembleResult.errorWarningMsg.empty())
    {
        const char* errorData = disassembleResult.errorWarningMsg.c_str();
        result->errorWarningMsg = CopyString(errorData);
    }
    if (!disassembleResult.target.empty())
    {
        result->targetSize = disassembleResult.target.size();
        binaryArray = new char[result->targetSize];
        std::copy(disassembleResult.target.data(), disassembleResult.target.data() + result->targetSize, binaryArray);

        result->target = binaryArray;      
    }

    result->hasError = disassembleResult.hasError;
    result->isText = disassembleResult.isText;
}

void FreeResources()
{
    delete[] binaryArray;
    delete[] msgArray;
}
