/**
 * Autogenerated by Thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */

package test.fixtures.refs;

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
@com.facebook.swift.codec.ThriftStruct(value="AdaptedStructWithTerseInternBox", builder=AdaptedStructWithTerseInternBox.Builder.class)
public final class AdaptedStructWithTerseInternBox implements com.facebook.thrift.payload.ThriftSerializable {
    @ThriftConstructor
    public AdaptedStructWithTerseInternBox(
        @com.facebook.swift.codec.ThriftField(value=1, name="field1", requiredness=Requiredness.TERSE) final test.fixtures.refs.Empty field1,
        @com.facebook.swift.codec.ThriftField(value=2, name="field2", requiredness=Requiredness.TERSE) final test.fixtures.refs.MyField field2
    ) {
        this.field1 = field1;
        this.field2 = field2;
    }
    
    @ThriftConstructor
    protected AdaptedStructWithTerseInternBox() {
      this.field1 = test.fixtures.refs.Empty.defaultInstance();
      this.field2 = test.fixtures.refs.MyField.defaultInstance();
    }

    public static Builder builder() {
      return new Builder();
    }

    public static Builder builder(AdaptedStructWithTerseInternBox other) {
      return new Builder(other);
    }

    public static class Builder {
        private test.fixtures.refs.Empty field1 = test.fixtures.refs.Empty.defaultInstance();
        private test.fixtures.refs.MyField field2 = test.fixtures.refs.MyField.defaultInstance();
    
        @com.facebook.swift.codec.ThriftField(value=1, name="field1", requiredness=Requiredness.TERSE)    public Builder setField1(test.fixtures.refs.Empty field1) {
            this.field1 = field1;
            return this;
        }
    
        public test.fixtures.refs.Empty getField1() { return field1; }
    
            @com.facebook.swift.codec.ThriftField(value=2, name="field2", requiredness=Requiredness.TERSE)    public Builder setField2(test.fixtures.refs.MyField field2) {
            this.field2 = field2;
            return this;
        }
    
        public test.fixtures.refs.MyField getField2() { return field2; }
    
        public Builder() { }
        public Builder(AdaptedStructWithTerseInternBox other) {
            this.field1 = other.field1;
            this.field2 = other.field2;
        }
    
        @ThriftConstructor
        public AdaptedStructWithTerseInternBox build() {
            AdaptedStructWithTerseInternBox result = new AdaptedStructWithTerseInternBox (
                this.field1,
                this.field2
            );
            return result;
        }
    }
    
    public static final Map<String, Integer> NAMES_TO_IDS = new HashMap<>();
    public static final Map<String, Integer> THRIFT_NAMES_TO_IDS = new HashMap<>();
    public static final Map<Integer, TField> FIELD_METADATA = new HashMap<>();
    private static final TStruct STRUCT_DESC = new TStruct("AdaptedStructWithTerseInternBox");
    private final test.fixtures.refs.Empty field1;
    public static final int _FIELD1 = 1;
    private static final TField FIELD1_FIELD_DESC = new TField("field1", TType.STRUCT, (short)1);
        private final test.fixtures.refs.MyField field2;
    public static final int _FIELD2 = 2;
    private static final TField FIELD2_FIELD_DESC = new TField("field2", TType.STRUCT, (short)2);
    static {
      NAMES_TO_IDS.put("field1", 1);
      THRIFT_NAMES_TO_IDS.put("field1", 1);
      FIELD_METADATA.put(1, FIELD1_FIELD_DESC);
      NAMES_TO_IDS.put("field2", 2);
      THRIFT_NAMES_TO_IDS.put("field2", 2);
      FIELD_METADATA.put(2, FIELD2_FIELD_DESC);
    }
    
    @Nullable
    @com.facebook.swift.codec.ThriftField(value=1, name="field1", requiredness=Requiredness.TERSE)
    public test.fixtures.refs.Empty getField1() { return field1; }

    
    @Nullable
    @com.facebook.swift.codec.ThriftField(value=2, name="field2", requiredness=Requiredness.TERSE)
    public test.fixtures.refs.MyField getField2() { return field2; }

    @java.lang.Override
    public String toString() {
        ToStringHelper helper = toStringHelper(this);
        helper.add("field1", field1);
        helper.add("field2", field2);
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
    
        AdaptedStructWithTerseInternBox other = (AdaptedStructWithTerseInternBox)o;
    
        return
            Objects.equals(field1, other.field1) &&
            Objects.equals(field2, other.field2) &&
            true;
    }

    @java.lang.Override
    public int hashCode() {
        return Arrays.deepHashCode(new java.lang.Object[] {
            field1,
            field2
        });
    }

    
    public static com.facebook.thrift.payload.Reader<AdaptedStructWithTerseInternBox> asReader() {
      return AdaptedStructWithTerseInternBox::read0;
    }
    
    public static AdaptedStructWithTerseInternBox read0(TProtocol oprot) throws TException {
      TField __field;
      oprot.readStructBegin(AdaptedStructWithTerseInternBox.NAMES_TO_IDS, AdaptedStructWithTerseInternBox.THRIFT_NAMES_TO_IDS, AdaptedStructWithTerseInternBox.FIELD_METADATA);
      AdaptedStructWithTerseInternBox.Builder builder = new AdaptedStructWithTerseInternBox.Builder();
      while (true) {
        __field = oprot.readFieldBegin();
        if (__field.type == TType.STOP) { break; }
        switch (__field.id) {
        case _FIELD1:
          if (__field.type == TType.STRUCT) {
            test.fixtures.refs.Empty field1 = test.fixtures.refs.Empty.read0(oprot);
            builder.setField1(field1);
          } else {
            TProtocolUtil.skip(oprot, __field.type);
          }
          break;
        case _FIELD2:
          if (__field.type == TType.STRUCT) {
            test.fixtures.refs.MyField field2 = test.fixtures.refs.MyField.read0(oprot);
            builder.setField2(field2);
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
      int structStart = 0;
      int pos = 0;
      com.facebook.thrift.protocol.ByteBufTProtocol p = (com.facebook.thrift.protocol.ByteBufTProtocol) oprot;
      java.util.Objects.requireNonNull(field1, "field1 must not be null");
      structStart = p.mark();
        oprot.writeFieldBegin(FIELD1_FIELD_DESC);
        pos = p.mark();
        this.field1.write0(oprot);
        if (p.mark() - pos > p.getEmptyStructSize()) {
          p.writeFieldEnd();    
        } else {
          p.rollback(structStart);
        }    
      java.util.Objects.requireNonNull(field2, "field2 must not be null");
      structStart = p.mark();
        oprot.writeFieldBegin(FIELD2_FIELD_DESC);
        pos = p.mark();
        this.field2.write0(oprot);
        if (p.mark() - pos > p.getEmptyStructSize()) {
          p.writeFieldEnd();    
        } else {
          p.rollback(structStart);
        }    
      oprot.writeFieldStop();
      oprot.writeStructEnd();
    }

    private static class _AdaptedStructWithTerseInternBoxLazy {
        private static final AdaptedStructWithTerseInternBox _DEFAULT = new AdaptedStructWithTerseInternBox.Builder().build();
    }
    
    public static AdaptedStructWithTerseInternBox defaultInstance() {
        return  _AdaptedStructWithTerseInternBoxLazy._DEFAULT;
    }
}
