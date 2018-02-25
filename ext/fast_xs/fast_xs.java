import org.jruby.Ruby;
import org.jruby.RubyString;
import org.jruby.RubyFixnum;
import org.jruby.anno.JRubyMethod;
import org.jruby.runtime.ThreadContext;
import org.jruby.runtime.builtin.IRubyObject;

public class fast_xs {
    static String[] cp_1252 = {
        "8364",     /* 128 => 8364, euro sign */
        null,       /* 129 => 129,  pass-through */
        "8218",     /* 130 => 8218, single low-9 quotation mark */
        "402",      /* 131 =>  402, latin small letter f with hook */
        "8222",     /* 132 => 8222, double low-9 quotation mark */
        "8230",     /* 133 => 8230, horizontal ellipsis */
        "8224",     /* 134 => 8224, dagger */
        "8225",     /* 135 => 8225, double dagger */
        "710",      /* 136 =>  710, modifier letter circumflex accent */
        "8240",     /* 137 => 8240, per mille sign */
        "352",      /* 138 =>  352, latin capital letter s with caron */
        "8249",     /* 139 => 8249, single left-pointing angle quotation mark */
        "338",      /* 140 =>  338, latin capital ligature oe */
        null,       /* 141 =>  141, pass-through */
        "381",      /* 142 =>  381, latin capital letter z with caron */
        null,       /* 143 =>  143, pass-through */
        null,       /* 144 =>  144, pass-through */
        "8216",     /* 145 => 8216, left single quotation mark */
        "8217",		/* 146 => 8217, right single quotation mark */
        "8220",		/* 147 => 8220, left double quotation mark */
        "8221",		/* 148 => 8221, right double quotation mark */
        "8226",		/* 149 => 8226, bullet */
        "8211",		/* 150 => 8211, en dash */
        "8212",		/* 151 => 8212, em dash */
        "732",		/* 152 =>  732, small tilde */
        "8482",		/* 153 => 8482, trade mark sign */
        "353",		/* 154 =>  353, latin small letter s with caron */
        "8250",		/* 155 => 8250, single right-pointing angle quotation mark */
        "339",		/* 156 =>  339, latin small ligature oe */
        null,		/* 157 =>  157, pass-through */
        "382",		/* 158 =>  382, latin small letter z with caron */
        "376"		/* 159 =>  376} latin capital letter y with diaeresis */
    };

    public static boolean validValue(int n) {
        return (n >= 0x20 && n <= 0xD7FF) ||
            (n >= 0xE000 && n <= 0xFFFD) ||
            (n >= 0x10000 && n <= 0x10FFFF);
    }

    private static String encode(int n) {
        if (n < 128) {
            if (n >= 0x20 || n == 9 || n == 10 || n == 13) {
                if (n == 34) {
                    return "&quot;";
                } else if (n == 39) {
                    return "&#x27;";
                } else if (n == 38) {
                    return "&amp;";
                } else if (n == 60) {
                    return "&lt;";
                } else if (n == 62) {
                    return "&gt;";
                }
                return null;
            }
            return "*";
        }
        if (n >= 128 && n <= 159) {
            int i = n - 128;
            String entity = cp_1252[i];
            if(entity == null)
                return null;
            StringBuilder sb = new StringBuilder();
            sb.append("&#");
            sb.append(entity);
            sb.append(";");
            return sb.toString();
        }
        if(validValue(n)) {
            StringBuilder sb = new StringBuilder();
            sb.append("&#");
            sb.append(n);
            sb.append(";");
            return sb.toString();
        }
        return "*";
    }

    @JRubyMethod(name = "fast_xs")
    public static IRubyObject fast_xs_p(ThreadContext context, IRubyObject self) {
        Ruby runtime = context.runtime;
        RubyString str, result, replace, c, u;
        RubyFixnum index;
        int tmp;

        str = (RubyString) self;
        Object[] unpacked;
        result = str.strDup(context.runtime);

        try {
            u = RubyString.newString(context.runtime, "U*");
            unpacked = str.unpack(u).toArray();
        } catch (Exception e) {
            c = RubyString.newString(context.runtime, "C*");
            unpacked = str.unpack(c).toArray();
        }

        int length = 0;
        for (int i = 0; i < unpacked.length; i++) {
            tmp = ((Long) unpacked[i]).intValue();
            String s = encode(tmp);
            if(s == null) {
                length += 1;
                continue;
            }
            replace = RubyString.newString(runtime, s);
            index = RubyFixnum.newFixnum(runtime, length);
            result.op_aset19(context, index, replace);
            length += s.length();
        }
        return result;
    }

    public static void init(Ruby runtime) {
        runtime.getString().defineAnnotatedMethods(fast_xs.class);
    }
}
