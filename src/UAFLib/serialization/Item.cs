using System.Collections.Generic;
using ProtoBuf;

namespace UAFLib.serialization
{
    [ProtoContract]
    class ItemCollection
    {
        [ProtoMember(1)]
        public Dictionary<string, object> itemData;
    }
}
