import fs from 'fs';
import en_us from '../assets/strings/en_us.js';
import pt_br from '../assets/strings/pt_br.js';

function assert(a) {
    if (!a) {
        throw new Error();
    }
}

function main() {
    const key_count = Object.keys(en_us).length;

    const langs = [['en_us', en_us], ['pt_br', pt_br]];

    for (const [_, values] of langs) {
        assert(key_count === Object.keys(values).length);
    }

    const lang_count = langs.length;

    /** @type string[] */
    const headerLines = [];
    headerLines.push("#ifndef GEN_STRINGS_H");
    headerLines.push("#define GEN_STRINGS_H");
    headerLines.push("");
    headerLines.push(`#define GEN_STRING_LANG_LENGTH ${lang_count}`);
    headerLines.push(`#define GEN_STRING_KEY_LENGTH ${key_count}`);
    headerLines.push("");
    headerLines.push("enum gen_string_lang {");
    for (const [key] of langs) {
        headerLines.push(`    GEN_STRING_LANG_${key.toUpperCase()},`);
    }
    headerLines.push("};");
    headerLines.push("");
    headerLines.push("enum gen_string_key {");
    for (const key of Object.keys(langs[0][1])) {
        headerLines.push(`    GEN_STRING_KEY_${key.toUpperCase()},`);
    }
    headerLines.push("};");
    headerLines.push("");
    headerLines.push("extern const char *gen_strings[GEN_STRING_LANG_LENGTH]");
    headerLines.push("                              [GEN_STRING_KEY_LENGTH];");
    headerLines.push("");
    headerLines.push("#endif // GEN_STRINGS_H");

    /** @type string[] */
    const implLines = [];
    implLines.push("#include <gen/strings.h>");
    implLines.push("");
    implLines.push("const char *gen_strings[GEN_STRING_LANG_LENGTH]");
    implLines.push("                       [GEN_STRING_KEY_LENGTH] = {");
    implLines.push("");
    for (const [lang, values] of langs) {
        implLines.push(`{  // ---------------------------- ${lang} -- START`);
        implLines.push("");
        for (const key of Object.keys(values)) {
            const value = values[key];
            implLines.push(`// ${lang} ${key}`);
            implLines.push(`"${value}",`);
            implLines.push("");
        }
        implLines.push(`}, // ---------------------------- ${lang} -- END`);
        implLines.push("");
    }
    implLines.push("};");

    const genFileH = `src_gen/gen/strings.h`;
    fs.writeFileSync(genFileH, headerLines.join('\n'));

    const genFileC = `src_gen/gen/strings.c`;
    fs.writeFileSync(genFileC, implLines.join('\n'));
}

main();
