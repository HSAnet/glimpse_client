import simplejson
import re

className = "ManualRequest"
inputFile = "manualrequest.json"

variables = list()
properties = list()
setters = list()
getters = list()
signals = list()
implementations = list()
jsoncode = list()

def capitalize(line):
    return ' '.join([s[0].upper() + s[1:] for s in line.split(' ')])

def adjustKeyName(key):
    exp = re.compile(r"-(\w)")

    result = exp.search(key)
    while result is not None:
        letter = result.group(1)
        key = key.replace(result.group(0), letter.upper())

        result = exp.search(key)

    return key

def renderProperty(key, value):
    jsonKey = key
    key = adjustKeyName(key)

    if type(value) is dict:
        return

    if len(value) is 0:
        value = "QString"

    isEnum = value.startswith("enum:")
    if isEnum:
        value = value[5:]
        isConst = False
    else:
        isConst = not value.startswith("int")
    
    getters.append("%s %s() const;" % (value, key))

    if isConst:
        params = "const %s& %s" % (value, key)
    else:
        params = "%s %s" % (value, key)

    setter = (capitalize(key), params)
    setters.append("void set%s(%s);" % setter)

    properties.append("Q_PROPERTY(%s %s READ %s WRITE set%s NOTIFY %sChanged)" % ("int" if isEnum else value, key, key, capitalize(key), key))
    variables.append("%s %s;" % (className+"::"+value if isEnum else value, key))
    signals.append("void %sChanged(%s);" % (key, params))

    implementations.append(
        "%s %s::%s() const {\n"
        "  return d->%s;\n"
        "}\n"
        % (className+"::"+value if isEnum else value, className, key, key)
        )

    implementations.append(
        "void %s::set%s(%s) {\n"
        "  if (d->%s != %s) {\n"
        "    d->%s = %s;\n"
        "    emit %sChanged(%s);\n"
        "  }\n"
        "}\n"
        % (className, capitalize(key), params,
           key, key,
           key, key,
           key, key)
        )

    jsonvalue = key + "()"
    if isEnum:
        jsonvalue = r'enumToString(%s, "%s", %s)' % (className, value, jsonvalue)

    jsoncode.append(
        r'  data.insert("%s", %s);'
        % (jsonKey, jsonvalue)
        )

def main():
    with file(inputFile, "r") as f:
        root = simplejson.load(f)

        jsoncode.append(
            "QVariant %s::toVariant() const {" % className
            )
        jsoncode.append("  QVariantMap data;")

        for key in root.keys():
            renderProperty(key, root[key])

        jsoncode.append("  return data;")
        jsoncode.append("}")

    for s in properties:
        print s
    print

    for s in getters:
        print s
    print
    
    for s in setters:
        print s
    print

    for s in signals:
        print s
    print

    for s in variables:
        print s
    print

    for s in implementations:
        print s
    print

    for s in jsoncode:
        print s
    print

main()
