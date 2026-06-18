using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TriviaClient
{
    public class Deserializer
    {
        public static JObject ParseResponse(string json)
        {
            try
            {
                return JObject.Parse(json);
            }
            catch (Exception e)
            {
                throw new Exception("Parse failed: " + e.Message);
            }
        }

        public static string GetField(string json, string field)
        {
            JObject obj = ParseResponse(json);
            return obj[field]?.ToString() ?? "";
        }
    }
}
