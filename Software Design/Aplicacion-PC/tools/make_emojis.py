#!/usr/bin/python3

import json
import pystache
import base64

TEMPLATE = """
static emoji_t EMOJIES[] = {
    {{#emojies}}
    {
        .name = "{{{name}}}",
        .shortname = "{{{shortname}}}",
        .category = "{{{category}}}",
        .data = "{{{data}}}",
        .data_size = {{{data_size}}},
        .w = 128,
        .h = 128,
    },
    {{/emojies}}
};

static const char *CATEGORIES[] = {
{{#categories}}
    "{{{name}}}",
{{/categories}}
    NULL
};
"""

def encode_bin(data):
    return base64.b85encode(data, pad=True)


categories = set()
emojies = json.load(open("data/emojis/emoji.json"))
for k, e in emojies.items():
    f = open("data/emojis/%s.png" % e["unicode"], 'rb')
    data = f.read()
    e["data"] = encode_bin(data)
    e["data_size"] = len(data)
    categories.add(e["category"])

emojies = emojies.values()
categories = [{'name':x} for x in categories]

out = pystache.render(TEMPLATE, {'emojies': emojies,
                                 'categories': categories})

dst = open("src/emoji_data.inl", "w")
print(out, file=dst)
