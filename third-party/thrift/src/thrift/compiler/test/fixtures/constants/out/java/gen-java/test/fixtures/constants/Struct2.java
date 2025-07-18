/**
 * Autogenerated by Thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */

package test.fixtures.constants;

import com.facebook.swift.codec.*;
import com.facebook.swift.codec.ThriftField.Requiredness;
import com.facebook.swift.codec.ThriftField.Recursiveness;
import com.google.common.collect.*;
import java.util.*;
import javax.annotation.Nullable;
import org.apache.thrift.*;
import org.apache.thrift.TException;
import org.apache.thrift.transport.*;
import org.apache.thrift.protocol.*;
import org.apache.thrift.protocol.TProtocol;
import static com.google.common.base.MoreObjects.toStringHelper;
import static com.google.common.base.MoreObjects.ToStringHelper;

@SwiftGenerated
@com.facebook.swift.codec.ThriftStruct(value="struct2", builder=Struct2.Builder.class)
public final class Struct2 implements com.facebook.thrift.payload.ThriftSerializable {
    @ThriftConstructor
    public Struct2(
        @com.facebook.swift.codec.ThriftField(value=1, name="a", requiredness=Requiredness.NONE) final int a,
        @com.facebook.swift.codec.ThriftField(value=2, name="b", requiredness=Requiredness.NONE) final String b,
        @com.facebook.swift.codec.ThriftField(value=3, name="c", requiredness=Requiredness.NONE) final test.fixtures.constants.Struct1 c,
        @com.facebook.swift.codec.ThriftField(value=4, name="d", requiredness=Requiredness.NONE) final List<Integer> d
    ) {
        this.a = a;
        this.b = b;
        this.c = c;
        this.d = d;
    }
    
    @ThriftConstructor
    protected Struct2() {
      this.a = 0;
      this.b = null;
      this.c = null;
      this.d = null;
    }

    public static Builder builder() {
      return new Builder();
    }

    public static Builder builder(Struct2 other) {
      return new Builder(other);
    }

    public static class Builder {
        private int a = 0;
        private String b = null;
        private test.fixtures.constants.Struct1 c = null;
        private List<Integer> d = null;
    
        @com.facebook.swift.codec.ThriftField(value=1, name="a", requiredness=Requiredness.NONE)    public Builder setA(int a) {
            this.a = a;
            return this;
        }
    
        public int getA() { return a; }
    
            @com.facebook.swift.codec.ThriftField(value=2, name="b", requiredness=Requiredness.NONE)    public Builder setB(String b) {
            this.b = b;
            return this;
        }
    
        public String getB() { return b; }
    
            @com.facebook.swift.codec.ThriftField(value=3, name="c", requiredness=Requiredness.NONE)    public Builder setC(test.fixtures.constants.Struct1 c) {
            this.c = c;
            return this;
        }
    
        public test.fixtures.constants.Struct1 getC() { return c; }
    
            @com.facebook.swift.codec.ThriftField(value=4, name="d", requiredness=Requiredness.NONE)    public Builder setD(List<Integer> d) {
            this.d = d;
            return this;
        }
    
        public List<Integer> getD() { return d; }
    
        public Builder() { }
        public Builder(Struct2 other) {
            this.a = other.a;
            this.b = other.b;
            this.c = other.c;
            this.d = other.d;
        }
    
        @ThriftConstructor
        public Struct2 build() {
            Struct2 result = new Struct2 (
                this.a,
                this.b,
                this.c,
                this.d
            );
            return result;
        }
    }
    
    public static final Map<String, Integer> NAMES_TO_IDS = new HashMap<>();
    public static final Map<String, Integer> THRIFT_NAMES_TO_IDS = new HashMap<>();
    public static final Map<Integer, TField> FIELD_METADATA = new HashMap<>();
    private static final TStruct STRUCT_DESC = new TStruct("struct2");
    private final int a;
    public static final int _A = 1;
    private static final TField A_FIELD_DESC = new TField("a", TType.I32, (short)1);
        private final String b;
    public static final int _B = 2;
    private static final TField B_FIELD_DESC = new TField("b", TType.STRING, (short)2);
        private final test.fixtures.constants.Struct1 c;
    public static final int _C = 3;
    private static final TField C_FIELD_DESC = new TField("c", TType.STRUCT, (short)3);
        private final List<Integer> d;
    public static final int _D = 4;
    private static final TField D_FIELD_DESC = new TField("d", TType.LIST, (short)4);
    static {
      NAMES_TO_IDS.put("a", 1);
      THRIFT_NAMES_TO_IDS.put("a", 1);
      FIELD_METADATA.put(1, A_FIELD_DESC);
      NAMES_TO_IDS.put("b", 2);
      THRIFT_NAMES_TO_IDS.put("b", 2);
      FIELD_METADATA.put(2, B_FIELD_DESC);
      NAMES_TO_IDS.put("c", 3);
      THRIFT_NAMES_TO_IDS.put("c", 3);
      FIELD_METADATA.put(3, C_FIELD_DESC);
      NAMES_TO_IDS.put("d", 4);
      THRIFT_NAMES_TO_IDS.put("d", 4);
      FIELD_METADATA.put(4, D_FIELD_DESC);
    }
    
    
    @com.facebook.swift.codec.ThriftField(value=1, name="a", requiredness=Requiredness.NONE)
    public int getA() { return a; }

    
    @Nullable
    @com.facebook.swift.codec.ThriftField(value=2, name="b", requiredness=Requiredness.NONE)
    public String getB() { return b; }

    
    @Nullable
    @com.facebook.swift.codec.ThriftField(value=3, name="c", requiredness=Requiredness.NONE)
    public test.fixtures.constants.Struct1 getC() { return c; }

    
    @Nullable
    @com.facebook.swift.codec.ThriftField(value=4, name="d", requiredness=Requiredness.NONE)
    public List<Integer> getD() { return d; }

    @java.lang.Override
    public String toString() {
        ToStringHelper helper = toStringHelper(this);
        helper.add("a", a);
        helper.add("b", b);
        helper.add("c", c);
        helper.add("d", d);
        return helper.toString();
    }

    @java.lang.Override
    public boolean equals(java.lang.Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
    
        Struct2 other = (Struct2)o;
    
        return
            Objects.equals(a, other.a) &&
            Objects.equals(b, other.b) &&
            Objects.equals(c, other.c) &&
            Objects.equals(d, other.d) &&
            true;
    }

    @java.lang.Override
    public int hashCode() {
        return Arrays.deepHashCode(new java.lang.Object[] {
            a,
            b,
            c,
            d
        });
    }

    
    public static com.facebook.thrift.payload.Reader<Struct2> asReader() {
      return Struct2::read0;
    }
    
    public static Struct2 read0(TProtocol oprot) throws TException {
      TField __field;
      oprot.readStructBegin(Struct2.NAMES_TO_IDS, Struct2.THRIFT_NAMES_TO_IDS, Struct2.FIELD_METADATA);
      Struct2.Builder builder = new Struct2.Builder();
      while (true) {
        __field = oprot.readFieldBegin();
        if (__field.type == TType.STOP) { break; }
        switch (__field.id) {
        case _A:
          if (__field.type == TType.I32) {
            int a = oprot.readI32();
            builder.setA(a);
          } else {
            TProtocolUtil.skip(oprot, __field.type);
          }
          break;
        case _B:
          if (__field.type == TType.STRING) {
            String b = oprot.readString();
            builder.setB(b);
          } else {
            TProtocolUtil.skip(oprot, __field.type);
          }
          break;
        case _C:
          if (__field.type == TType.STRUCT) {
            test.fixtures.constants.Struct1 c = test.fixtures.constants.Struct1.read0(oprot);
            builder.setC(c);
          } else {
            TProtocolUtil.skip(oprot, __field.type);
          }
          break;
        case _D:
          if (__field.type == TType.LIST) {
            List<Integer> d;
                {
                TList _list = oprot.readListBegin();
                d = new ArrayList<Integer>(Math.max(0, _list.size));
                for (int _i = 0; (_list.size < 0) ? oprot.peekList() : (_i < _list.size); _i++) {
                    
                    int _value1 = oprot.readI32();
                    d.add(_value1);
                }
                oprot.readListEnd();
                }
            builder.setD(d);
          } else {
            TProtocolUtil.skip(oprot, __field.type);
          }
          break;
        default:
          TProtocolUtil.skip(oprot, __field.type);
          break;
        }
        oprot.readFieldEnd();
      }
      oprot.readStructEnd();
      return builder.build();
    }

    public void write0(TProtocol oprot) throws TException {
      oprot.writeStructBegin(STRUCT_DESC);
      oprot.writeFieldBegin(A_FIELD_DESC);
      oprot.writeI32(this.a);
      oprot.writeFieldEnd();
      if (b != null) {
        oprot.writeFieldBegin(B_FIELD_DESC);
        oprot.writeString(this.b);
        oprot.writeFieldEnd();
      }
      if (c != null) {
        oprot.writeFieldBegin(C_FIELD_DESC);
        this.c.write0(oprot);
        oprot.writeFieldEnd();
      }
      if (d != null) {
        oprot.writeFieldBegin(D_FIELD_DESC);
        List<Integer> _iter0 = d;
        oprot.writeListBegin(new TList(TType.I32, _iter0.size()));
            for (int _iter1 : _iter0) {
              oprot.writeI32(_iter1);
            }
            oprot.writeListEnd();
        oprot.writeFieldEnd();
      }
      oprot.writeFieldStop();
      oprot.writeStructEnd();
    }

    private static class _Struct2Lazy {
        private static final Struct2 _DEFAULT = new Struct2.Builder().build();
    }
    
    public static Struct2 defaultInstance() {
        return  _Struct2Lazy._DEFAULT;
    }
}
