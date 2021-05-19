import org.jruby.Ruby;
import org.jruby.RubyString;
import org.jruby.RubyFixnum;
import org.jruby.anno.JRubyMethod;
import org.jruby.runtime.ThreadContext;
import org.jruby.runtime.builtin.IRubyObject;
import org.jcodings.Encoding;
import java.nio.charset.Charset;

public class fast_xs_extra {
    @JRubyMethod(name = "fast_xs_html")
    public static IRubyObject fast_xs_html_p(ThreadContext context, IRubyObject self) {
        RubyString replace;
        RubyFixnum index;
        RubyString selfStr = (RubyString) self;
        String str = selfStr.toString();
        Ruby runtime = context.runtime;
        StringBuilder sb = new StringBuilder();
        Encoding enc = selfStr.getEncoding();

        for (int i = 0; i < str.length(); i++) {
            char c = str.charAt(i);
            String entity;
            if (c == '&') {
                entity = "&amp;";
            } else if (c == '<') {
                entity = "&lt;";
            } else if (c == '>') {
                entity = "&gt;";
            } else if (c == '"') {
                entity = "&quot;";
            } else if (c == '\'') {
                entity = "&#x27;";
            } else {
              entity = String.valueOf(c);
            }

            sb.append(entity);
        }
        return RubyString.newString(context.runtime, sb.toString(), enc);
    }


    private static boolean cgiOk(char x) {
        return ((x >= 'a' && x <= 'z') ||
          (x >= 'A' && x <= 'Z') ||
          (x >= '0' && x <= '9') ||
          (x == '.' || x == '-' || x == '_'));
    }

    private static IRubyObject fast_xs_uri_encode_p(ThreadContext context, IRubyObject self, boolean escapeSpace) {
        RubyString replace;
        RubyFixnum index;
        RubyString selfStr = (RubyString) self;
        String str = selfStr.toString();
        Ruby runtime = context.runtime;
        StringBuilder sb = new StringBuilder();
        Encoding enc = selfStr.getEncoding();
        for (int i = 0; i < str.length(); i++) {
            char c = str.charAt(i);
            String entity;
            if (cgiOk(c)) {
                entity = String.valueOf(c);
            } else if (escapeSpace && c == ' ') {
                entity = "+";
            } else {
                RubyFixnum tmpIndex = RubyFixnum.newFixnum(runtime, i);
                RubyString tmp = (RubyString)selfStr.op_aref19(context, tmpIndex);
                Object[] unpacked;
                RubyString b = RubyString.newString(context.runtime, "C*");
                unpacked = tmp.unpack(b).toArray();

                entity = "";
                for (int j = 0; j < unpacked.length; j++) {
                    entity += "%" + Integer.toHexString(((Long) unpacked[j]).intValue()).toUpperCase();
                }
            }
            sb.append(entity);
        }
        return RubyString.newString(context.runtime, sb.toString(), enc);
    }

    @JRubyMethod(name = "fast_xs_cgi")
    public static IRubyObject fast_xs_cgi_p(ThreadContext context, IRubyObject self) {
        return fast_xs_uri_encode_p(context, self, true);
    }

    @JRubyMethod(name = "fast_xs_url")
    public static IRubyObject fast_xs_url_p(ThreadContext context, IRubyObject self) {
        return fast_xs_uri_encode_p(context, self, false);
    }

    private static boolean isHex(String str, int i) {
        return (str.length() > i) &&
            ((str.charAt(i) >= '0' && str.charAt(i) <= '9') ||
            (str.charAt(i) >= 'A' && str.charAt(i) <= 'F'));
    }

    @JRubyMethod(name = "fast_uxs_cgi")
    public static IRubyObject fast_uxs_cgi_p(ThreadContext context, IRubyObject self) {
      String str = ((RubyString) self).toString();
      Ruby runtime = context.runtime;
      RubyString replace;
      RubyFixnum index;
      StringBuilder sb = new StringBuilder();

      int length = 0;
      for (int i = 0; i < str.length(); i++) {
          char c = str.charAt(i);
          String entity;
          if (c == '+') {
              entity = " ";
          } else if (c == '%' && isHex(str, i + 1) && isHex(str, i + 2)) {
              entity = "" + (char)Integer.parseInt(str.substring(i + 1, i + 3), 16);
              i += 2;
          } else {
              entity = String.valueOf(c);
          }
          sb.append(entity);
      }
      return RubyString.newString(context.runtime, sb.toString());
    }

    public static void init(Ruby runtime) {
        runtime.getString().defineAnnotatedMethods(fast_xs_extra.class);
    }
}
