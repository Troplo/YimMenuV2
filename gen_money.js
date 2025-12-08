const fs = require('fs');
const path = require('path');

const inputFile = path.join(__dirname, 'src/game/gta/Natives.hpp');
const outputCppFile = path.join(__dirname, 'native_impl.cpp');
const outputHppFile = path.join(__dirname, 'native_impl.hpp');

const content = fs.readFileSync(inputFile, 'utf8');

// Extract MONEY namespace
const moneyNamespaceMatch = content.match(/namespace MONEY\s*{([\s\S]*?)^}/m);
if (!moneyNamespaceMatch) {
    console.error('MONEY namespace not found!');
    process.exit(1);
}
const moneyContent = moneyNamespaceMatch[1];

// Match FORCEINLINE functions
const funcRegex = /^\s*FORCEINLINE\s+constexpr\s+void\s+(\w+)\(([\s\S]*?)\)\s*{[\s\S]*?}/gm;

let match;
let cppOutput = '#include "native_impl.hpp"\n\n';
let hppOutput = '#pragma once\n\nclass ParagonMoney\n{\npublic:\n';

let functionInfos = [];

while ((match = funcRegex.exec(moneyContent)) !== null) {
    const funcName = match[1];
    const args = match[2].trim();

    if (!funcName.includes('EARN') && !funcName.includes('SPEND')) continue;

    const callType = funcName.includes('EARN') ? 'EarnCash' : 'SpendCash';

    // Parse arguments into array of { type, name }
    const argList = args
        ? args.split(',').map(a => {
            const parts = a.trim().split(/\s+/);
            const name = parts.pop();
            const type = parts.join(' ');
            return { type, name };
        })
        : [];

    // Determine the main argument and index
    let mainArg = 'p1';
    let argIndex = 0;
    const amountParam = argList.findIndex(a => a.name === 'amount');
    if (amountParam !== -1) {
        mainArg = 'amount';
        argIndex = amountParam;
    } else if (argList.length > 0) {
        mainArg = argList[0].name;
        argIndex = 0;
    }

    functionInfos.push({ funcName, callType, mainArg, argIndex });

    // Header
    hppOutput += `    static void ${funcName}(rage::scrNativeCallContext* src);\n`;
}

hppOutput += '    static void Register();\n';
hppOutput += '};\n';

// Generate .cpp functions
for (const f of functionInfos) {
    cppOutput += `void ParagonMoney::${f.funcName}(rage::scrNativeCallContext* src)\n{\n`;
    cppOutput += `    LOG(INFO) << __FUNCTION__;\n`
    cppOutput += `    const auto ${f.mainArg} = src->GetArg<int>(${f.argIndex});\n`;

    let extraArgs;
    if (f.callType === 'EarnCash') {
        extraArgs = 'false, "MONEY_EARN_JOBS"_J, "TROPLOROS_MONEY_MULTIPLIER_JOB_HARD_LIMIT"_J';
    } else {
        extraArgs = 'false, "MONEY_SPEND_JOBS"_J';
    }

    cppOutput += `    ${f.callType}(${f.mainArg}, ${extraArgs});\n`;
    cppOutput += `}\n\n`;
}

// Generate Register() function
cppOutput += 'void ParagonMoney::Register()\n{\n';
for (const f of functionInfos) {
    cppOutput += `    NativeHooks::AddHook(NativeHooks::ALL_SCRIPTS, NativeIndex::${f.funcName}, ParagonMoney::${f.funcName});\n`;
}
cppOutput += '}\n';

fs.writeFileSync(outputCppFile, cppOutput, 'utf8');
fs.writeFileSync(outputHppFile, hppOutput, 'utf8');

console.log(`Generated ${outputCppFile} and ${outputHppFile} with Register() function`);
