/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.facebook.thrift.protocol;

import io.netty.buffer.ByteBuf;
import io.netty.buffer.ByteBufAllocator;
import io.netty.handler.codec.base64.Base64;
import io.netty.util.ReferenceCountUtil;
import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;
import java.util.ArrayDeque;
import java.util.Map;
import org.apache.thrift.TException;
import org.apache.thrift.protocol.TField;
import org.apache.thrift.protocol.TList;
import org.apache.thrift.protocol.TMap;
import org.apache.thrift.protocol.TMessage;
import org.apache.thrift.protocol.TProtocolException;
import org.apache.thrift.protocol.TSet;
import org.apache.thrift.protocol.TStruct;
import org.apache.thrift.protocol.TType;

public abstract class ByteBufAbstractTSimpleJSONProtocol extends ByteBufTProtocol {

  private static final byte[] COMMA = new byte[] {','};
  private static final byte[] COLON = new byte[] {':'};
  private static final byte[] LBRACE = new byte[] {'{'};
  private static final byte[] RBRACE = new byte[] {'}'};
  private static final byte[] LBRACKET = new byte[] {'['};
  private static final byte[] RBRACKET = new byte[] {']'};
  private static final byte[] QUOTE = new byte[] {'"'};
  private static final byte[] BOOL_TRUE = "true".getBytes(StandardCharsets.UTF_8);
  private static final byte[] BOOL_FALSE = "false".getBytes(StandardCharsets.UTF_8);
  private static final byte WHITE_SPACE = ' ';
  private static final byte TAB = '\t';
  private static final byte NEW_LINE = '\n';
  private static final byte CARRIAGE_RETURN = '\r';

  private static final TStruct ANONYMOUS_STRUCT = new TStruct();
  private static final TField ANONYMOUS_FIELD = new TField();
  private static final TMessage EMPTY_MESSAGE = new TMessage();
  private static final TSet EMPTY_SET = new TSet();
  private static final TList EMPTY_LIST = new TList();
  private static final TMap EMPTY_MAP = new TMap();
  private static final String LIST = "list";
  private static final String SET = "set";
  private static final String MAP = "map";

  private static final long VERSION = 1;

  protected class Context {
    protected void write() throws TException {}

    /** Returns whether the current value is a key in a map */
    protected boolean isMapKey() {
      return false;
    }
  }

  protected class ListContext extends Context {
    protected boolean first_ = true;

    protected void write() throws TException {
      if (first_) {
        first_ = false;
      } else {
        getByteBuf().writeBytes(COMMA);
      }
    }
  }

  protected class StructContext extends Context {
    protected boolean first_ = true;
    protected boolean colon_ = true;

    protected void write() throws TException {
      if (first_) {
        first_ = false;
        colon_ = true;
      } else {
        getByteBuf().writeBytes(colon_ ? COLON : COMMA);
        colon_ = !colon_;
      }
    }
  }

  protected class MapContext extends StructContext {
    protected boolean isKey = true;

    @Override
    protected void write() throws TException {
      super.write();
      isKey = !isKey;
    }

    protected boolean isMapKey() {
      // we want to coerce map keys to json strings regardless
      // of their type
      return isKey;
    }
  }

  protected final Context BASE_CONTEXT = new Context();

  /** Stack of nested contexts that we may be in. */
  protected ArrayDeque<Context> writeContextStack_ = new ArrayDeque<>();

  /** Current context that we are in */
  protected Context writeContext_ = BASE_CONTEXT;

  /** Push a new write context onto the stack. */
  protected void pushWriteContext(Context c) {
    writeContextStack_.push(writeContext_);
    writeContext_ = c;
  }

  /** Pop the last write context off the stack */
  protected void popWriteContext() {
    writeContext_ = writeContextStack_.pop();
  }

  /** Used to make sure that we are not encountering a map whose keys are containers */
  protected void assertContextIsNotMapKey(String invalidKeyType) throws CollectionMapKeyException {
    if (writeContext_.isMapKey()) {
      throw new CollectionMapKeyException(
          "Cannot serialize a map with keys that are of type " + invalidKeyType);
    }
  }

  public void writeMessageBegin(TMessage message) throws TException {
    getByteBuf().writeBytes(LBRACKET);
    pushWriteContext(new ListContext());
    writeString(message.name);
    writeByte(message.type);
    writeI32(message.seqid);
  }

  public void writeMessageEnd() throws TException {
    popWriteContext();
    getByteBuf().writeBytes(RBRACKET);
  }

  public void writeStructBegin(TStruct struct) throws TException {
    writeContext_.write();
    getByteBuf().writeBytes(LBRACE);
    pushWriteContext(new StructContext());
  }

  public void writeStructEnd() throws TException {
    popWriteContext();
    getByteBuf().writeBytes(RBRACE);
  }

  public void writeFieldBegin(TField field) throws TException {
    // Note that extra type information is omitted in JSON!
    writeString(field.name);
  }

  public void writeFieldEnd() {}

  public void writeFieldStop() {}

  public void writeMapBegin(TMap map) throws TException {
    assertContextIsNotMapKey(MAP);
    writeContext_.write();
    getByteBuf().writeBytes(LBRACE);
    pushWriteContext(new MapContext());
    // No metadata!
  }

  public void writeMapEnd() throws TException {
    popWriteContext();
    getByteBuf().writeBytes(RBRACE);
  }

  public void writeListBegin(TList list) throws TException {
    assertContextIsNotMapKey(LIST);
    writeContext_.write();
    getByteBuf().writeBytes(LBRACKET);
    pushWriteContext(new ListContext());
    // No metadata!
  }

  public void writeListEnd() throws TException {
    popWriteContext();
    getByteBuf().writeBytes(RBRACKET);
  }

  public void writeSetBegin(TSet set) throws TException {
    assertContextIsNotMapKey(SET);
    writeContext_.write();
    getByteBuf().writeBytes(LBRACKET);
    pushWriteContext(new ListContext());
    // No metadata!
  }

  public void writeSetEnd() throws TException {
    popWriteContext();
    getByteBuf().writeBytes(RBRACKET);
  }

  public void writeBool(boolean b) throws TException {
    writeContext_.write();
    getByteBuf().writeBytes(b ? BOOL_TRUE : BOOL_FALSE);
  }

  public void writeByte(byte b) throws TException {
    writeI32(b);
  }

  public void writeI16(short i16) throws TException {
    writeI32(i16);
  }

  public void writeI32(int i32) throws TException {
    if (writeContext_.isMapKey()) {
      writeString(Integer.toString(i32));
    } else {
      writeContext_.write();
      _writeStringData(Integer.toString(i32));
    }
  }

  public void _writeStringData(String s) throws TException {
    byte[] b = s.getBytes(StandardCharsets.UTF_8);
    getByteBuf().writeBytes(b);
  }

  public void writeI64(long i64) throws TException {
    if (writeContext_.isMapKey()) {
      writeString(Long.toString(i64));
    } else {
      writeContext_.write();
      _writeStringData(Long.toString(i64));
    }
  }

  public void writeFloat(float flt) throws TException {
    if (writeContext_.isMapKey()) {
      writeString(Float.toString(flt));
    } else {
      writeContext_.write();
      _writeStringData(Float.toString(flt));
    }
  }

  public void writeDouble(double dub) throws TException {
    if (writeContext_.isMapKey()) {
      writeString(Double.toString(dub));
    } else {
      writeContext_.write();
      _writeStringData(Double.toString(dub));
    }
  }

  public void writeString(String str) throws TException {
    writeContext_.write();
    int length = str.length();
    StringBuffer escape = new StringBuffer(length + 16);
    escape.append((char) QUOTE[0]);
    for (int i = 0; i < length; ++i) {
      char c = str.charAt(i);
      switch (c) {
        case '"':
        case '\\':
          escape.append('\\');
          escape.append(c);
          break;
        case '\b':
          escape.append('\\');
          escape.append('b');
          break;
        case '\f':
          escape.append('\\');
          escape.append('f');
          break;
        case '\n':
          escape.append('\\');
          escape.append('n');
          break;
        case '\r':
          escape.append('\\');
          escape.append('r');
          break;
        case '\t':
          escape.append('\\');
          escape.append('t');
          break;
        default:
          // Control characters! According to JSON RFC u0020 (space)
          if (c < ' ') {
            String hex = Integer.toHexString(c);
            escape.append('\\');
            escape.append('u');
            for (int j = 4; j > hex.length(); --j) {
              escape.append('0');
            }
            escape.append(hex);
          } else {
            escape.append(c);
          }
          break;
      }
    }
    escape.append((char) QUOTE[0]);
    _writeStringData(escape.toString());
  }

  public abstract void writeBinary(ByteBuffer bin) throws TException;

  /** Reading methods. */
  private class StructReadContext {
    final Map<String, Integer> namesToIds;
    final Map<String, Integer> javaNamesToIds;
    final Map<Integer, TField> fieldMetadata;

    StructReadContext(
        Map<String, Integer> namesToIds,
        Map<String, Integer> javaNamesToIds,
        Map<Integer, TField> fieldMetadata) {
      this.namesToIds = namesToIds;
      this.javaNamesToIds = javaNamesToIds;
      this.fieldMetadata = fieldMetadata;
    }
  }

  private ArrayDeque<StructReadContext> readContexts = new ArrayDeque<>();

  private StructReadContext currentReadContext = null;

  // Holds up to one byte from the transport
  protected class LookaheadReader {
    private boolean hasData_;
    private byte[] data_ = new byte[1];

    private boolean hasNextData_ = false;
    private byte[] nextData_ = new byte[1];

    // Return and consume the next byte to be read, either taking it from the
    // data buffer if present or getting it from the transport otherwise.
    protected byte read(boolean skip) throws TException {
      if (hasNextData_) {
        byte result = data_[0];
        data_[0] = nextData_[0];
        hasNextData_ = false;
        return result;
      }

      if (hasData_) {
        hasData_ = false;
      } else {
        readDirectly(data_, skip);
      }
      return data_[0];
    }

    protected byte read() throws TException {
      return read(true);
    }

    // Return the next byte to be read without consuming, filling the data
    // buffer if it has not been filled already.
    private byte peek() throws TException {
      if (!hasData_) {
        read();
      }
      hasData_ = true;
      return data_[0];
    }

    private byte peekNext() throws TException {
      if (!hasNextData_) {
        peek();
        readDirectly(nextData_);
        hasNextData_ = true;
      }
      return nextData_[0];
    }

    private void readDirectly(byte[] data) {
      readDirectly(data, true);
    }

    private void readDirectly(byte[] data, boolean skip) {
      byte b;
      do {
        byteBuf.readBytes(data, 0, 1);
        b = data[0];
      } while (skip && (b == WHITE_SPACE || b == TAB || b == NEW_LINE || b == CARRIAGE_RETURN));
    }

    protected boolean bytesAvailable() {
      return getByteBuf().readableBytes() > 0;
    }
  }

  // Stack of nested contexts that we may be in
  private ArrayDeque<JSONBaseContext> contextStack_ = new ArrayDeque<>();

  // Current context that we are in
  private JSONBaseContext context_ = new JSONBaseContext();

  // Base class for tracking JSON contexts that may require inserting/reading
  // additional JSON syntax characters
  // This base context does nothing.
  protected class JSONBaseContext {
    protected void write() throws TException {}

    protected void read() throws TException {}

    protected boolean escapeNum() {
      return false;
    }
  }

  // Context for JSON lists. Will insert/read commas before each item except
  // for the first one
  protected class JSONListContext extends JSONBaseContext {
    private boolean first_ = true;

    @Override
    protected void write() throws TException {
      if (first_) {
        first_ = false;
      } else {
        getByteBuf().writeBytes(COMMA);
      }
    }

    @Override
    protected void read() throws TException {
      if (first_) {
        first_ = false;
      } else {
        readJSONSyntaxChar(COMMA);
      }
    }
  }

  // Push a new JSON context onto the stack.
  private void pushContext(JSONBaseContext c) {
    contextStack_.push(context_);
    context_ = c;
  }

  // Pop the last JSON context off the stack
  private void popContext() {
    context_ = contextStack_.pop();
  }

  // Context for JSON records. Will insert/read colons before the value portion
  // of each record pair, and commas before each key except the first. In
  // addition, will indicate that numbers in the key position need to be
  // escaped in quotes (since JSON keys must be strings).
  protected class JSONPairContext extends JSONBaseContext {
    private boolean first_ = true;
    private boolean colon_ = true;

    @Override
    protected void write() throws TException {
      if (first_) {
        first_ = false;
        colon_ = true;
      } else {
        getByteBuf().writeBytes(colon_ ? COLON : COMMA);
        colon_ = !colon_;
      }
    }

    @Override
    protected void read() throws TException {
      if (first_) {
        first_ = false;
      } else {
        byte[] expected = colon_ ? COLON : COMMA;
        colon_ = !colon_;
        readJSONSyntaxChar(expected);
      }
    }

    @Override
    protected boolean escapeNum() {
      try {
        return reader_.peek() == QUOTE[0];
      } catch (Exception e) {
        throw new RuntimeException(e);
      }
    }
  }

  // Reader that manages a 1-byte buffer
  private LookaheadReader reader_ = new LookaheadReader();

  // Temporary buffer used by several methods
  private byte[] tmpbuf_ = new byte[4];

  private static final String ESCAPE_CHARS = "\"\\/bfnrt";
  private static final byte[] ESCSEQ = new byte[] {'\\', 'u', '0', '0'};
  private static final int DEF_STRING_SIZE = 16;
  private static final byte[] ESCAPE_CHAR_VALS = {
    '"', '\\', '/', '\b', '\f', '\n', '\r', '\t',
  };
  private static final byte[] ZERO = new byte[] {'0'};

  // Read in a JSON string, unescaping as appropriate..
  private ByteBuf readJSONString() throws TException {
    ByteBuf arr = ByteBufAllocator.DEFAULT.buffer(DEF_STRING_SIZE);
    readJSONSyntaxChar(QUOTE);
    while (true) {
      byte ch = reader_.read(false);
      if (ch == QUOTE[0]) {
        break;
      }
      if (ch == ESCSEQ[0]) {
        ch = reader_.read(false);
        if (ch == ESCSEQ[1]) {
          readJSONSyntaxChar(ZERO);
          readJSONSyntaxChar(ZERO);
          ch = (byte) ((hexVal(reader_.read(false)) << 4) + hexVal(reader_.read(false)));
        } else {
          int off = ESCAPE_CHARS.indexOf(ch);
          if (off == -1) {
            throw new TProtocolException(TProtocolException.INVALID_DATA, "Expected control char");
          }
          ch = ESCAPE_CHAR_VALS[off];
        }
      }
      arr.writeByte(ch);
    }
    return arr;
  }

  // Read a byte that must match b[0]; otherwise an exception is thrown.
  // Marked protected to avoid synthetic accessor in JSONListContext.read
  // and JSONPairContext.read
  protected void readJSONSyntaxChar(byte[] b) throws TException {
    readJSONSyntaxString(b);
  }

  protected void readJSONSyntaxString(byte[] expected) throws TException {
    int i = 0;
    do {
      char ch = (char) reader_.read();
      if (ch != expected[i]) {
        throw new TProtocolException(
            TProtocolException.INVALID_DATA,
            String.format(
                "Unexpected character '%s' at position %d (expected %s from '%s')",
                ch, i, expected[i], new String(expected)));
      }
      i++;
    } while (i < expected.length);
  }

  // Convert a byte containing a hex char ('0'-'9' or 'a'-'f') into its
  // corresponding hex value
  private static final byte hexVal(byte ch) throws TException {
    if ((ch >= '0') && (ch <= '9')) {
      return (byte) ((char) ch - '0');
    } else if ((ch >= 'a') && (ch <= 'f')) {
      return (byte) ((char) ch - 'a' + 10);
    } else {
      throw new TProtocolException(TProtocolException.INVALID_DATA, "Expected hex character");
    }
  }

  // Return true if the given byte could be a valid part of a JSON number.
  private boolean isJSONNumeric(byte b) {
    switch (b) {
      case '+':
      case '-':
      case '.':
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case 'E':
      case 'e':
        return true;
    }
    return false;
  }

  // Read in a sequence of characters that are all valid in JSON numbers. Does
  // not do a complete regex check to validate that this is actually a number.
  private String readJSONNumericChars() throws TException {
    StringBuilder strbld = new StringBuilder();
    while (true) {
      if (!reader_.bytesAvailable()) {
        break;
      }
      byte ch = reader_.peek();
      if (!isJSONNumeric(ch)) {
        break;
      }
      strbld.append((char) reader_.read());
    }
    return strbld.toString();
  }

  // Read in a JSON number. If the context dictates, read in enclosing quotes.
  private long readJSONInteger() throws TException {
    context_.read();
    if (context_.escapeNum()) {
      readJSONSyntaxChar(QUOTE);
    }
    String str = readJSONNumericChars();
    if (context_.escapeNum()) {
      readJSONSyntaxChar(QUOTE);
    }
    try {
      return Long.valueOf(str);
    } catch (NumberFormatException ex) {
      throw new TProtocolException(
          TProtocolException.INVALID_DATA, "Bad data encounted in numeric data");
    }
  }

  // Read in a JSON double value. Throw if the value is not wrapped in quotes
  // when expected or if wrapped in quotes when not expected.
  private double readJSONDouble() throws TException {
    ByteBuf arr = null;
    try {
      context_.read();
      if (reader_.peek() == QUOTE[0]) {
        arr = readJSONString();
        return Double.parseDouble(arr.toString(StandardCharsets.UTF_8));
      } else {
        if (context_.escapeNum()) {
          // This will throw - we should have had a quote if escapeNum == true
          readJSONSyntaxChar(QUOTE);
        }
        try {
          return Double.parseDouble(readJSONNumericChars());
        } catch (NumberFormatException ex) {
          throw new TProtocolException(
              TProtocolException.INVALID_DATA, "Bad data encounted in numeric data");
        }
      }
    } finally {
      if (arr != null) {
        ReferenceCountUtil.safeRelease(arr);
      }
    }
  }

  // Read in a JSON float value. Throw if the value is not wrapped in quotes
  // when expected or if wrapped in quotes when not expected.
  protected float readJSONFloat() throws TException {
    ByteBuf arr = null;
    try {
      context_.read();
      if (reader_.peek() == QUOTE[0]) {
        arr = readJSONString();
        String s;
        try {
          s = arr.toString(StandardCharsets.UTF_8);
        } catch (Exception e) {
          throw new TException(e);
        }
        return Float.parseFloat(s);
      } else {
        if (context_.escapeNum()) {
          // This will throw - we should have had a quote if escapeNum == true
          readJSONSyntaxChar(QUOTE);
        }
        try {
          return Float.parseFloat(readJSONNumericChars());
        } catch (NumberFormatException ex) {
          throw new TProtocolException(
              TProtocolException.INVALID_DATA, "Bad data encounted in numeric data");
        }
      }
    } finally {
      if (arr != null) {
        ReferenceCountUtil.safeRelease(arr);
      }
    }
  }

  // Read in a JSON string containing base-64 encoded data and decode it.
  private byte[] readJSONBase64() throws TException {
    ByteBuf arr = null;
    ByteBuf decode = null;
    try {
      arr = readJSONString();
      decode = Base64.decode(arr);
      byte[] b = new byte[decode.readableBytes()];
      decode.readBytes(b);
      return b;
    } finally {
      if (arr != null) {
        ReferenceCountUtil.safeRelease(arr);
      }

      if (decode != null) {
        ReferenceCountUtil.safeRelease(decode);
      }
    }
  }

  private void readJSONObjectStart() throws TException {
    context_.read();
    readJSONSyntaxChar(LBRACE);

    pushContext(new JSONPairContext());
  }

  private void readJSONObjectEnd() throws TException {
    readJSONSyntaxChar(RBRACE);
    popContext();
  }

  private void readJSONArrayStart() throws TException {
    context_.read();
    readJSONSyntaxChar(LBRACKET);

    pushContext(new JSONListContext());
  }

  private void readJSONArrayEnd() throws TException {
    readJSONSyntaxChar(RBRACKET);
    popContext();
  }

  @Override
  public TMessage readMessageBegin() throws TException {
    readJSONArrayStart();
    ByteBuf buf = null;
    try {
      buf = readJSONString();
      String name = buf.toString(StandardCharsets.UTF_8);
      byte type = (byte) readJSONInteger();
      int seqid = (int) readJSONInteger();
      return new TMessage(name, type, seqid);
    } catch (Exception e) {
      throw new TException();
    } finally {
      if (buf != null) {
        ReferenceCountUtil.safeRelease(buf);
      }
    }
  }

  @Override
  public void readMessageEnd() throws TException {
    readJSONArrayEnd();
  }

  @Override
  public TStruct readStructBegin(
      Map<String, Integer> namesToIds,
      Map<String, Integer> thriftNamesToIds,
      Map<Integer, TField> fieldMetadata)
      throws TException {
    if (currentReadContext != null) {
      readContexts.push(currentReadContext);
    }

    currentReadContext = new StructReadContext(namesToIds, thriftNamesToIds, fieldMetadata);

    readJSONObjectStart();
    return ANONYMOUS_STRUCT;
  }

  @Override
  public TStruct readStructBegin() {
    throw new UnsupportedOperationException(
        "You need to specify a \"field name\" -> \"field Id\" mapping to deserialize with"
            + " TSimpleJSON");
  }

  @Override
  public void readStructEnd() throws TException {
    currentReadContext = readContexts.isEmpty() ? null : readContexts.pop();
    readJSONObjectEnd();
  }

  @Override
  public TField readFieldBegin() throws TException {
    while (true) {
      if (reader_.peek() == RBRACE[0]) {
        return new TField("", TType.STOP, (short) 0);
      }

      context_.read();
      ByteBuf buf = null;
      try {
        buf = readJSONString();
        final String fieldName = buf.toString(StandardCharsets.UTF_8);
        Integer fieldId = currentReadContext.namesToIds.get(fieldName);
        if (fieldId == null) {
          // backward compatibility
          fieldId = currentReadContext.javaNamesToIds.get(fieldName);
          if (fieldId == null) {
            return new TField(fieldName, getTypeIDForPeekedByte(reader_.peekNext()), (short) 0);
          }
        }
        return currentReadContext.fieldMetadata.get(fieldId);
      } catch (Exception e) {
        throw new TException(e);
      } finally {
        if (buf != null) {
          ReferenceCountUtil.safeRelease(buf);
        }
      }
    }
  }

  @Override
  public void readFieldEnd() throws TException {}

  public TMap readMapBegin() throws TException {
    readJSONObjectStart();
    return new TMap(getTypeIDForPeekedByte(reader_.peek()), TType.STOP, -1);
  }

  public boolean peekMap() throws TException {
    return reader_.peek() != RBRACE[0];
  }

  public void readMapEnd() throws TException {
    readJSONObjectEnd();
  }

  public TList readListBegin() throws TException {
    readJSONArrayStart();
    return new TList(getTypeIDForPeekedByte(reader_.peek()), -1);
  }

  public boolean peekList() throws TException {
    return reader_.peek() != RBRACKET[0];
  }

  public void readListEnd() throws TException {
    readJSONArrayEnd();
  }

  public TSet readSetBegin() throws TException {
    readJSONArrayStart();
    return new TSet(getTypeIDForPeekedByte(reader_.peek()), -1);
  }

  public boolean peekSet() throws TException {
    return reader_.peek() != RBRACKET[0];
  }

  public void readSetEnd() throws TException {
    readJSONArrayEnd();
  }

  @Override
  public boolean readBool() throws TException {
    context_.read();
    byte peek = reader_.peek();
    boolean hasQuote = peek == QUOTE[0];
    if (hasQuote) {
      readJSONSyntaxChar(QUOTE);
    }

    boolean value;
    peek = reader_.peek();
    if (peek == BOOL_TRUE[0]) {
      readJSONSyntaxString(BOOL_TRUE);
      value = true;
    } else if (peek == BOOL_FALSE[0]) {
      readJSONSyntaxString(BOOL_FALSE);
      value = false;
    } else {
      throw new TException(
          String.format("unexpected first char '%c', it doesn't match 'true' nor 'false'", peek));
    }

    if (hasQuote) {
      readJSONSyntaxChar(QUOTE);
    }
    return value;
  }

  @Override
  public byte readByte() throws TException {
    return (byte) readJSONInteger();
  }

  @Override
  public short readI16() throws TException {
    return (short) readJSONInteger();
  }

  @Override
  public int readI32() throws TException {
    return (int) readJSONInteger();
  }

  @Override
  public long readI64() throws TException {
    return (long) readJSONInteger();
  }

  @Override
  public double readDouble() throws TException {
    return readJSONDouble();
  }

  @Override
  public float readFloat() throws TException {
    return readJSONFloat();
  }

  public String readString() throws TException {
    context_.read();
    ByteBuf buf = null;
    try {
      buf = readJSONString();
      return buf.toString(StandardCharsets.UTF_8);
    } catch (Exception e) {
      throw new TException(e);
    } finally {
      if (buf != null) {
        ReferenceCountUtil.safeRelease(buf);
      }
    }
  }

  public abstract ByteBuffer readBinary() throws TException;

  @Override
  public void skipBinary() throws TException {
    // use readString to skip bytes to prevent an error when Base64 encoding
    readString();
  }

  public static class CollectionMapKeyException extends TException {
    public CollectionMapKeyException(String message) {
      super(message);
    }
  }

  protected static final byte getTypeIDForPeekedByte(byte peekedByte) throws TException {
    switch (peekedByte) {
      case '}':
      case ']':
        return TType.STOP;

      case '{':
        return TType.STRUCT;

      case '[':
        return TType.LIST;

      case 't':
      case 'f':
        return TType.BOOL;

      case '+':
      case '-':
      case '.':
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        return TType.DOUBLE;

      case '"':
        return TType.STRING;

      default:
        throw new TProtocolException(
            TProtocolException.NOT_IMPLEMENTED, "Unrecognized peeked byte: " + (char) peekedByte);
    }
  }

  @Override
  public final void writeBinaryAsByteBuf(ByteBuf bin) throws TException {
    throw new UnsupportedOperationException(
        "JSON protocols do not support direct memory operations");
  }

  @Override
  public final ByteBuf getWritableBinaryAsByteBuf(int size) throws TException {
    throw new UnsupportedOperationException(
        "JSON protocols do not support direct memory operations");
  }

  @Override
  public final ByteBuf readBinaryAsSlice() throws TException {
    throw new UnsupportedOperationException(
        "JSON protocols do not support direct memory operations");
  }
}
