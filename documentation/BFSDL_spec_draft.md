# Binary Format Stream Definition Language

Version 0-DRAFT

Copyright 2018 Daniel Kristensen, Garmin Ltd, or its subsidiaries.
All rights reserved.

Redistribution and use in source (Markdown) and 'compiled' forms (HTML, PDF, PostScript, RTF, etc...) with or without modification, are permitted provided that the following conditions are met:

* Redistributions in source form (Markdown) must retain the above copyright notice, this list of conditions and the following disclaimer.

* Redistributions in compiled form (converted to HTML, PDF, etc...) must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote documents following this specification (or any extension thereof) without specific prior written permission.

THIS DOCUMENTATION IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS DOCUMENTATION, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

## 0 Foreword, Definitions, and Conventions
BFSDL is a general-purpose data-definition language that allows definition of the format of binary data in a stream.  It may also be used to describe data chunks of a fixed size, such as from a file or encapsulated in some other data wrapper.  This document describes the grammar of the language as it would appear in a stream of text; (perhaps stored in a file).

Terminology which is defined in the document by a specific section number appears in Title Case.

This section defines the language of the *grammar used to describe the specification*; this is **NOT** the grammar of the BFSDL language itself.

### 0.1 Streams
There are two types of Streams discussed in the document; both are defined in this section to distinguish between them.

#### 0.1.1 Data Stream
A Data Stream is defined as a stream which contains data that may be described by a BFSDL Stream.

#### 0.1.2 BFSDL Stream
A BFSDL Stream is defined as a stream which contains text written in BFSDL and describes the interpretation of data in a Data Stream.

### 0.2 Document Syntax
This section describes the syntax of this document, not BFSDL.  Indented code blocks are used to visually describe or contain examples of the format of the stream being discussed in a section.

The word "content" is used to describe the content of the code blocks in this document, not the content of the BFSDL Stream or the Data Stream.

#### 0.2.1 Content Descriptors

Content is described by a sequence of characters from the set of lowercase letters (a-z) and a dash (-).  A number (0-9) may be appended to distinguish various instances of the content.  Examples follow:

    a-content-identifier
    another-content-identifier1

##### 0.2.1.1 Identified Content

Identified Content is defined as content which is represented by a single identifier.

##### 0.2.1.2 Text Content

Text Content is defined as content which is not represented by an identifier.

##### 0.2.1.3 Simple Content

Simple Content is defined as one instance of either Identified Content or Text Content

##### 0.2.1.3 Complex Content

Complex Content is defined as content which will be represented by the aggregation of multiple instances of Simple Content.

#### 0.2.2 Optional Content

Optional Content is Simple or complex content surrounded by square brackets:

    [optional-content]

#### 0.2.3 Field Names

Field Names are Simple Content written as a name surrounded by angle brackets:

    <field-name>

#### 0.2.4 Literal Text

Literal Text is Text Content surrounded by single-quotes; the Text Content will appear verbatim in the BFSDL Stream:

    'literal-text'

#### 0.2.4 Literal Numeric Text

Literal Numeric Text is Text Content represented in numeric form as a valid Unicode value in Base 10 (which may or may not include leading zeros); the Text Content will appear verbatim in the BFSDL Stream.  The value may be preceded by a lowercase x to indicate the value is in Base 16.  Both examples below are equivalent to Literal Text 'A': 

    65
    00065
    x41
    x0041

#### 0.2.5 Repeated Content

Repeated Content is enclosed in parentheses, followed by an ellipses:

    (repeated-content)...

The parentheses may be omitted if the content is in another container, in which case all contents of the container is repeated:

    [repeated-optional-content...]

The number of repetitions can be specified after the ellipses (example shown with 8 repetitions):

    (repeated-content)...8
    [repeated-optional-content...8]

#### 0.2.6 Switchable Content

Switchable Content is Complex Content which appears as one or more cases of Simple Content, in which one of the given cases will apply.  This is surrounded by parentheses, and separated by a vertical pipe between cases:

    (<field-1>|<field-2>|<field-3>)
    (<field-1>|'text1')

The parentheses may be omitted if the content is in another container:

    [<optional-field-1>|<optional-field-2>|<optional-field-3>]
    (<optional-field-1>|<optional-field-2>|<optional-field-3>)...

The parentheses and intermediate closing and opening brackets may be omitted if all content in the container is the same type and all embedded identifiers represent simple or aggregate content:

    <field-1|field-2|field-3>
    [<optional-field-1|optional-field-2|optional-field-3>]
    <the-only-option>

In some cases, such as Literal Numeric Text, multiple forms of the same content may be represented as Switchable Content.

##### NOTES

1. Since Switchable Content will only contain Simple Types, this means it cannot be used to directly represent Switchable Content or other Complex Content as one of the choices.  To do this, the Complex Content will be defined as Identified Content.
2. The single-option form is defined to simplify formation of Wildcard Content

#### 0.2.7 Defining Complex Content as Identified Content

Identifiers may be represented as an expression separated by a colon followed by an equals sign; the new identifier appears on the left hand side, and the Complex Content appears on the right:

    new-identifier := <field-1>'some text'

#### 0.2.7.1 Defining Switchable Complex Content as Identified Content

An Identifier definition may be expressed as multiple lines, with values on the right hand side of the `:=` being aligned, and continuation lines being indented.  Each value is to be interpreted like Switchable Content for the definition, so that one of the given cases will apply.

Example definition:

    new-identifier := <field-1>'some text'
                      <field-2a>'some other even longer text'
                          <field-2b>
                      <field-3>

This is equivalent to the following:

    case-1 := <field-1>'some text'
    case-2 := <field-2a>'some other even longer text'<field-2b>
    case-3 := <field-3>
    new-identifier := (<case-1>|<case-2>|<case-3>)

#### 0.2.7.2 Extending Switchable Complex Content

An existing definition of Switchable Complex Content may be extended using `+=`.

Example:

    new-identifier := <field-1>'some text'
                   := <field-2a>'some other even longer text'<field-2b>

    new-identifier += <field-3>

This is equivalent to the example in 0.2.7.1

#### 0.2.8 Wildcard Content

Wildcard Content is enclosed in parentheses:

* The Wildcard Content will include lists of Switchable Content, each of which may either be listed explicitly or using an identifier defined as Switchable Content.
* The Wildcard Content will represent data found in the content, matched either by the content listed at the beginning or an asterisk.
  * When Switchable Content is listed, all cases found in the data are represented
  * When an asterisk is used, any data matches. 
* The Wildcard Content may include modifiers using a modifier keyword followed by a list of Switchable Content, which is listed explicitly or using an identifier defined as Switchable Content.
* The Wildcard Content may exclude any of the listed content via "except" modifier.
* The Wildcard Content may allow any of the listed content "allow", if otherwise excluded by "except". 

For example:

    (*)
    (switched-content-identifier except 'foo')
    (* except ('baz'|'baz'))
    (* except foo-baz-bar allow 'bar')

#### 0.2.9 Comments

Comments may be used to help describe the text.  These begin with a `#` and continue to the end of the line:

    # Comment text taking the whole line
    (*)  # Comment text after the end of the line
    '#'  # Comment after Literal Text containing a hash

Hashes in Literal Text do not begin a comment. 

#### 0.2.10 Literal Ranges

A set of literals may be defined as Switchable Content by using the "to" keyword in parentheses; this may be combined with other values in the switch.  Each value in the range is a treated as a separate case:

    ('a' to 'z')
    ( 0-9|'.' )
    (1 to 3)  # same as (1|2|3)

## 1 BFSDL Stream Format Overview

The BFSDL Stream is comprised of a Header Definition, and a Data Definition.  Both sections are comprised of statements which are evaluated in sequence. 

## 2 Common Formatting Elements

Statements are evaluated with various rules, but several formatting elements are common to parsing all statements.

### 2.1 Line Breaks

Line breaks are a subset of white space characters which are used to make the BFSDL Stream easier to read and understand by a text viewer.

    new-line := (10|x0A)
    carriage-return := (13|x0D)
    line-break := (<new-line>|<carriage-return>)...

### 2.2 End of Stream

End of stream is a special identifier which has no content.  This is used to mark the end of the stream, which occurs when the parser stops reading from the BFSDL Stream.

    eos := ()

### 2.3 White Space

White space is Text Content which is useful for separating other forms of content, but otherwise does not affect the interpretation of the Data Stream.

    space := (32|x20)
    tab := (9|x09)
    intra-line-whitespace := (<space|tab>)...

    whitespace := (<intra-line-whitespace>|<line-break>|<eos>)...

`whitespace` is used sometimes throughout this Specification when needed to define a boundary for the system.

### 2.4 Statement Block

A statement block represents a collection of statements enclosed by curly braces:

    block-begin = '{'
	block-end = '}'
    statement-block := <block-begin>(* except <block-begin>)<block-end>

These may not be used by themselves, but appear as part of other constructs such as a class.

### 2.5 Word

Words represent a complete sequence of text that appears in the BFSDL Stream:

    word := ('a' to 'z'|'A' to 'Z'|'0' to '9'|'_')...

Words are used for identifiers and reserved keywords.  This is a very limited character set for consistency, and so that words only contain characters which fit into a single 8-bit code point.

## 3 Predefined Raw Data

To parse structures of a known type, some predefined data usually must be known beforehand and specified in the BFSDL Stream.

### 3.1 Numeric Literals

A Numeric Literal is defined as text representing a numeric value.

#### 3.1.1 Supported Number Systems

The numeral digits are defined for the set of number systems which are supported by BFSDL:

    numerals := ('0' to '1')...
                ('0' to '7')...
                ('0' to '9')...
                ('0' to '9'|'A'-'F'|'a-f')...

#### 3.1.2 Numeric Formatting Characters

The formatting characters which are common to multiple types of numeric literals are defined as:

    hash := '#'
    sign-marker := ('+'|'-')
    base-marker := ('b'|'o'|'d'|'x')
    period := '.'
    exponent-marker := ('^')

##### 3.1.2.1 Numeric Bases

The `base-marker` values correlate to a number system as follows:

* 'b' = Binary (radix 2)
* 'o' = Octal (radix 8)
* 'd' = Decimal (radix 10)
* 'x' = Hexadecimal (radix 16)

##### 3.1.2.2 Numeric Signing

The values for `<sign-marker>` shall correspond to the interpretive value of the numeral value defined collectively by both `<integral-component>` and `<fractional-component>`:

* `+` indicates that the value is positive
* `-` indicates that the value is negative

Zero is a special case in which the value can be either positive or negative.  Its effective value may be treated as identical for all cases in which the binary implementation makes no distinction.

#### 3.1.3 Numeric Literal Types

Numeric literals must contain an real integer component.  Optionally, they may also contain a real fractional component and a real exponential component, but all numeric components must be in the same base:

    integral-component := [<base-marker>':'][<sign-marker>]<numerals>
    fractional-component := <period><numerals>
    exponential-component := <exponent-marker>[<sign-marker>]<numerals>

    numeric-literal := <hash><integral-component>[<fractional-component>][<exponential-component>](<hash>|<whitespace>)

* The default value for `<sign-marker>` is `+`.
* The default value for `<base-marker>` is `d`.
* The default value for `<fractional-component>` is `0`
* The default value for `<exponential-component>` is `0`

The resulting number value is the result of the following *mathematical* expression:

    ( <integral-component>.<fractional-component> ) * ( b ^ <exponential-component> ),

where:
* Digits from `<integral-component>`, `<fractional-component>`, and `<exponential-component>` are interpreted according to `<base-marker>`
* `b` is the exponential power base defined by an associated floating-point bit format

#### 3.1.4 Exceptions for Support of Fractional Values

#### 3.1.4.1 Exponents and Floating Point Support

The `<exponential-component>` of a `<numeric-literal>` must be supported when the system defines support for at least one floating-point format.

Since the constraints of the exponential-component are dependent upon the definition of a floating-point bit format, `<exponential-component>` values are not allowed in a context where:

* There is no associated bit format defined.
* The associated bit format does not follow a supported floating-point type.

Although it may be tempting to do so as an extension, `<exponential-component>` must not be used as shorthand for allowing abbreviated numeric definitions because the resultant value may not have a deterministically-defined translation to a storage type.  A system that does not include floating-point support may fail to parse Numeric Literals with exponents.

For example, use `#1000000#` instead of `#10^6#`

#### 3.1.4.2 Limitations and Use of Fractional Values

The `<fractional-component>` of a `<numeric-literal>` shall only be supported when the `<base-marker>` indicates a radix which is an integral power of 2.  This is because radices which are not a power of 2 may not be able to be represented exactly in base 2 otherwise.

The system may store the intermediate representation of the `<fractional-component>` as fixed-point values until it must be converted to a specific storage format.  The value of 0.1 in base-10, for example, has an infinite number of binary digits when converted to base 2, and so it is not possible to store.

The digits of this number shall be interpreted a stream of digits mapped to their binary representation, not interpreted in the number system.

TODO: Need to remove this clause; see Issue GH-1

#### 3.1.5 Bitwise Representation of Numeric Literals

The bitwise representation of Numeric Literals is undefined until it is stored into or compared against a value with a defined bit format.  The system may place constraints on the range of supported values:

* When associating a Numeric Literal with a value that has a defined storage type, the system is only required to support values in the BFSDL Stream which can be accurately represented in the defined storage type.  The following bits are ignored in the translation to a bitwise storage implementation of the components:
    * Leading zero bits are ignored in `<integral-numeric-component>`
    * Trailing zero bits are ignored in `<fractional-numeric-component>`
    * Leading zero bits are ignored in `<exponential-component>`
* During association with a defined bit format, the system shall compress or expand values in the Numeric Literal to fit in the defined bit format, padding as necessary with bits to produce the most equivalent value for the storage format.  Values which are not able to be stored by the specified format are disallowed.

The implementation must support enough bits to represent the components of all defined integer and floating point types defined in this specification.

##### NOTE

Numeric Literals are essentially "type-less".  To use a Numeric Literal with a predefined storage type, it must be defined as a Constant (described later in the specification).

### 3.2 String Literals

A String Literal is defined as text which is bounded by double-quotes on a single line.

    double-quote := '"'
    string-literal-value := <double-quote>(* except (<double-quote>|<line-break>))<double-quote>

Matching of boundaries in a string literal takes precedence over matching of other bounded formatting elements, such as Statement Blocks.

#### NOTE

It is possible to build a string literal across multiple lines using Concatenation (defined later).

#### 3.2.1 Escape Sequences

Escape sequences may be used with `<string-literal-value>` to store characters in a string literal that are otherwise difficult to represent.

    back-slash := '\'
    escape-sequence := <back-slash><double-quote>            # Ex: \" = Unicode 34
                       <back-slash><back-slash>              # Ex: \\ = Unicode 92 
                       <back-slash>'t'                       # Ex: \t = Unicode 8
                       <back-slash>'r'                       # Ex: \r = Unicode 13
                       <back-slash>'n'                       # Ex: \n = Unicode 10
                       <back-slash>'a'(<base-16-digits>...2) # Ex: \aff = ASCII 255 
                       <back-slash>'w'(<base-16-digits>...2) # Ex: \w7f = Windows CP-1252 127 

Some escape sequences allow a variable number of digits to follow:

    escape-digits := <base-10-digits>...
    escape-sequence += <back-slash>[<escape-digits>]'x'(<base-16-digits>...2)  
        # Default for <escape-digits> = 2
        # Smallest supported range of <escape-digits> is 1-8 
        # Examples:
        #   \x34 = String code 52
        #   \4x1234 = String code 4660 
    escape-sequence += <back-slash>[<escape-digits>]'b'(<base-2-digits>....8)
        # Default for <escape-digits> = 8
        # Smallest supported range of <escape-digits> is 1-64
        # Examples:
        #   \b00110001 = String code 49
        #   \4b0001 = String code 1
        #   \16b0001001100110111 = String code 4919
    escape-sequence += <back-slash>[<escape-digits>]'u'(<base-16-digits>...4) 
        # Default for <escape-digits> = 4
        # Smallest supported range of <escape-digits> is 1-8
        # Examples:
        #   \u1234 = Unicode 4660
        #   \6u10FFFF = Unicode 1114111

Note that the numeric value of escape sequences corresponds to the *code point*, whose interpretation is dependent upon the encoding of the string. 

#### 3.2.2 String Encoding

String Literals must always use a defined coding:

    string-code := <word>

`<word>` could be any coding definedin Appendix D for the code() attribute, or by a supported extension (provided the usage is allowed by the extension according to the rules and constraints defined by the extension).

A String Literal may use the default code, or use `<string-code-attribute>` (attributes are listed in Appendix D) to specify the code:

    string-literal := <string-literal-value>
                      <string-literal-value>'.code("'<string-code>'")'

The value of `<string-code>` is used for any raw characters, as well as escape codes which do not have an implicit code (`\x`, `\b`, and `\u`).

The implicit code of escape sequences `\a` and `u` always override `<string-code>` for that symbol.

#### 3.2.3 Concatenation of String Literals

Multiple occurrences of String Literals in a single statement may be concatenated into a single String Literal:

    string-literal-element := (<string-literal-value>|<string-literal-with-code>)
    string-literal += [(<string-literal-element>'+')...]<string-literal-element>

#### 3.2.4 Bitwise Representation of String Literals

The bitwise representation of String Literals are undefined until it is stored into or compared against a value with a defined bit format.  The system may impose constraints:

* When associating a String Literal with a value that has a defined storage type, the system is only required to support values in the BFSDL Stream which can be accurately represented in the defined storage type.
* During association with a defined encoding format, the system shall disallow values which do not fit exactly.
* The system may impose any constraints on maximum string length
* The system may apply additional rules on undefined transcoding operations via extensions.  Examples:
    * Define one or more custom String Codes
    * Specify a value to use when transcoding operations which have no defined code point translation.
    * Specify additional conditions around usage of a transcoding operation to a custom String Code (ex: limiting code point conversion to a single direction).

## 4 BFSDL Stream Header Definition

At the beginning of the stream, a header must appear to set configuration settings for parsing the rest of the stream.

### 4.1 Format and Types of Settings

Settings in the header begin with a colon and continue until a `<line-break>` or the start of a new setting.  There are two types of settings:

#### 4.1.1 Keyword Setting

This is represented by a single keyword:

    keyword-setting := ':'<identifier>

#### 4.1.2 Configuration Setting

This is represented by a single keyword, followed by an equals sign, and a value:

    configuration-setting :=':'<identifier>'='(<string-literal>|<numeric-literal>)

The type and range of values that can be accepted are setting-specific.

#### 4.1.3 Blank Setting

This is represented by just a single colon:

    blank-setting := ':'

The purpose of this is to facilitate:

* Separating settings in a BFSDL Stream with line breaks for readability.
* Following the of the settings block for BFSDL Stream with no `<line-break>` to separate the block from following content..

### 4.2 Defined Settings

Setting `<identifier>` fields are case-sensitive and defined below by type.

#### 4.2.1 Keyword Settings

* BFSDL_HEADER - Must appear as the first non-whitespace element in the BFSDL Stream.
* END_HEADER - Must be the last non-blank setting, which marks the end of the block.

#### 4.2.2 Configuration Settings

Each configuration value must be specified exactly once per header section, unless otherwise stated.  Supported values are defined below:

* Version - Takes a Numeric Literal value specifying the minimum supported version of the specification to which the BFSDL Stream adheres to.
    * Default value = `#1#`
    * NOTE: The Version's value has no storage type
* BitBase - Takes a String Literal value `"Bit"` or `"Byte"` specifying the base data element for the Data Stream.
    * NOTE: Choosing `"Byte"` does not prevent the ability to handle bit fields, but bit fields must be a multiple of 8 bits wide.
    * Default value = `"Byte"`
* DefaultByteOrder - Takes a String Literal value `"LE"` (Little-Endian) or `"BE"`(Big-Endian) specifying the default endian-ness of byte-oriented numeric fields spanning multiple bytes.
    * Default value = `"LE"`
* DefaultBitOrder - Takes a String Literal value `"LE"` or `"BE"` specifying the default endian-ness of bit-oriented numeric fields with defined bits within each Byte element in the Data Stream.
    * Default value = `"LE"`
    * NOTE: This is independent of the byte-oriented computing system on which the parser is running, due to the nature of byte-oriented computer science (which must have a defined MSB and LSB).
    * NOTE: This setting has no effect on bit-oriented fields when used to describe data in a purely bit-oriented computing system.
* DefaultFloatFormat - Takes a String Literal value specifying the default floating point format
    * No default; if undefined, then all floating point bit formats must specify the format.
    * See Appendix C for allowed values.
* DefaultStringCode = Takes a String Literal value specifying the default value of `<string-code>` code for interpreting other String Literals found in the BFSDL Stream.
    * Default value = `ASCII`
    * NOTE: Before this configuration is parsed completely, the default value is used.  So any String Literals which require a different encoding must appear after this setting.
    * NOTE: Normal parsing rules for String Literals apply here, including escape sequences and attributes.  Examples of valid values ways to specify the UTF8 code:
        * `"UTF8"`
        * `"UTF\u0038"`
        * `"UTF\x38"` (`\x38` is interpreted as an ASCII code point, since the new default value has not taken effect)
        * `"UTF\x38".code("WCP1252")` ("code" is an attribute, see appendix D)
* DefaultStringPad - Takes a Numeric Literal value specifying the code to use for padding when converting a String Literal into a defined bit format.
    * Default Value = `#0#`
* CustomExtension - Takes a String Literal value specifying a custom extension which must be supported by the parser to interpret the BFSDL Stream correctly.
    * Default behavior: No custom extensions supported
    * Multiple settings of this type may be used
    * See Appendix B for more information about extensions

### 4.3 Implicit Header Specification

The BFSDL Stream Header may be omitted if the system implementing the BFSDL Stream parser supports an alternate method of specifying settings (ex: application configuration, hard-coded settings, etc...).

## 5 BFSDL Stream Data Definition

After the BFSDL Stream Header parameters are specified, the BFSDL Stream will contain a series of statements necessary to describe the format of the Data Stream.

The statements may contain definitions that allow aggregation of other statements into a named identifier; all statements at each level of aggregation is considered to be within the same scope.  A BFSDL Stream is itself a form of aggregation of statements, and so the statements at the top-level are said to be within the same scope as well.

* The top-level of the stream is defined as the Stream Scope.
* The BFSDL Header (if not provided externally from the BFSDL Stream) constitutes the Header Scope, which is contained in the Stream Scope.
* Other scopes will be defined later along with the statements that introduce an aggregation concept.

### 5.1 Statement Separation

Since white space is ignored, statements are separated explicitly until the end of the stream:

    end-of-statement := (';'|<eos>)

The content of each statement depends on the type of statement it is.

### 5.2 Bit Format Types

To parse the Data Stream, it is necessary to define types which correlate to a particular bit format.

#### 5.2.1 Fixed-Point Numeric Bit Format

Numeric bit formats are capable of storing real numeric values using a fixed-point representation.  The type identifiers are defined as follows:

    fixed-point-type := ('s'|'u')
    format-width := <base-10-digits>
    fractional-width := <base-10-digits>

    numeric-format := <fixed-point-type><format-width>[<period><fractional-width>]

* `<fixed-point-type>` specifies whether the type is unsigned or signed
* The sum of `<format-width>` and `<fractional-width>` specify the total number of elements the format occupies, according to the BitBase setting in the Header:
  * All parsers must support combinations with a total total number of storage bits up to and including 64 bits at a minimum.
  * For formats whose BitBase=`Byte`, the number of storage bits is 8 times the value specified
* The default value for `<fractional-width>` is `0`

##### 5.2.1.1 Representation of Signed Fixed-Point Numbers

If `<fixed-point-type>` = `s`, then the value is stored in 2's complement form; this does not change the total number of bits available for storage.

A couple of examples:

The following `s4` signed number value is calculated by the mathematical formula:

    4-bit data '1101' in base 2: 1101.0
    1101 = -1 * (1 * 2^3) + (1 * 2^2) + (0 * 2^1) + (1 * 2^0)
         = -8 + 4 + 1
         = -3

The following `s2.1` signed number value is calculated by the mathematical formula:

    3-bit data '101' in base 2: 10.1
    10.1 = -1 * (1 * 2^1) + (0 * 2^0) + (1 * 2^-1)
         = -2 + 0 + 0.5
         = -1.5

The following `s1` signed number values are calculated by the mathematical formulas:

    1-bit data '1' in base 2: 1
    1 = -1 * (1 * 2^0)
      = -1

    1-bit data '0' in base 2: 0
    0 = -1 * (0 * 2^0)
      = 0

##### 5.2.1.2 Examples of Fixed-Point Number Formats:

The following examples illustrate the interpretation and range of acceptable values for various formats:

* `u32` := 32-bit unsigned integer (0 to 4294967296)
* `s8` := 8-bit signed integer (-128 to 127)
* `u4.4` := 4.4-bit unsigned real (0.0 to 15.9375)
* `s2.2` := 2.2-bit signed real (-2.0 to 1.75)

#### 5.2.2 Floating-Point Numeric Bit Format

Floating point bit formats are capable of storing real numeric values using an externally-defined floating point representation.  The type identifiers are defined as follows:

    floating-point-type := <string-literal>
    floating-point-attribute := <period>'('<floating-point-type>')'
    floating-point-format := 'f'<floating-point-attribute>

`<floating-point-type>` specifies the format to follow.  These may be defined externally, but also see Appendix C for built-in extensions that define floating-point support.

NOTE: Because floating point numbers typically require floating point support from the computer system interpreting the data according to a specific standard, this section does not define any method to create arbitrary floating point number formats.  Many standard floating point number systems define special behavior for certain values such as +/-NaN, +/-Inf, and distinction between +/-0; and this behavior cannot be expressed concisely.

#### 5.2.3 String Bit Format

    string-bit-format := 'string'

This format may be modified by Attributes (see Appendix D).  The following built-in types are defined as shorthand for commonly used types:

    string-bit-format += 'cstring'  # Equivalent to string.term(), for C-style strings

    string-bit-format += 'pstring'  # Equivalent to string.plen(), for Pascal-style string

#### 5.2.4 Supported Bit Formats

Supported bit formats are defined as:

    bit-format := <numeric-format>
                  <floating-point-format>
                  <string-bit-format>
                  <custom-format>

NOTE: `<custom-format>` could be any format defined by an extension, provided the usage is allowed by the extension according to the rules and constraints defined by the extension.

### 5.3 Constants

A constant may be defined to a string literal, along with any associated attributes.

    constant := 'define'<whitespace><word><whitespace><bit-format><whitespace>(string-literal|numeric-literal)

Example of a Numeric Literal constant with no defined storage constraints:

    define my_version #1#;

Constants can be modified by the `type` attribute to specify a storage type:

    define my_version.type(u32) #1#;

See Apendix D for Attribute information.

### 5.4 Type Aliasing

Types may be aliased as a new identifier:

    alias-definition := 'alias'<whitespace><bit-format><whitespace><new-format>

Creates an alias `<new-format>` for the already-defined `<bit-format>` type.

Example:

    alias string.len(4).unterm() fourcc;

### 5.5 Field Definitions

Data fields describe a blob of data that may be found in the Data Stream

    data-field-definition := <bit-format><whitespace><word>

`<word>` is unique in its scope, to prevent ambiguity.  The Stream Scope must have at least one data field.

`<bit-format>` is a built-in format, an alias, or a format defined by a supported extension.

Examples:

    u8      version;
    pstring name;
    u32     crc;

## 6 Inclusion of External Definitions

TODO

## 7 Classes

TODO

## Appendix A: Change Log

1.00 Initial Release

## Appendix B: Extensions

TODO

## Appendix C: Extended Floating Point Support

This section describes additional floating-point support which is defined by extensions.

NOTE: Remember, extensions are not required to be supported by the parser.

### C.1 IEEE 754 Support

The extension `ieee754` is defined to identify that parsing the file requires supporting data formats from the IEEE 754 document.

#### C.1.1 Configuration

The following configuration settings may be specified in Header Scope to identify the support for floating point values

* IEEE754Version - Takes a Numeric Literal value specifying the minimum supported revision of the IEEE 754 Specification.
    * Default value = `#1985#`
        * NOTE: IEEE754Version's value has no storage type
    * Supported values:
        * `#1985#`
        * `#2008#`

#### C.1.2 IEEE 754 (1985)

When `IEEE754Version` is equal to `#1985#`, the following types from IEEE 754 (1985) are required to be supported:

* `single`
* `double`

#### C.1.3 IEEE 754 (2008)

When `IEEE754Version` is equal to `#2008#`, the following types from IEEE 754 (2008) are required to be supported (in addition to the types defined by the 1985 version):

* `binary16`
* `binary32` (equivalent to `single`)
* `binary64` (equivalent to `double`)
* `binary128`
* `binary256`
* `decimal32`
* `decimal64`
* `decimal128`

NOTE: Since these are inherently bit-oriented formats, the numeric suffix indicates the total number of bits of the field.

## Appendix D: Attributes

Attribute(s) are applied at the end of a value or statement where the format is used:

    attribute := '.'<attribute-identifier>'('<attribute-value>')'
                 '.'<attribute-identifier>'()'
    attribute-list := (<attribute>)...

Example:

    "HI".FOO().BAZ("BAR");
    u32.CHEESE(#1#) myField;

### D.1 String Attributes

All string attributes may be used in conjunction with each other, except as noted.

#### D.1.1 Encoding

`code(<encoding>)` - Specifies the encoding for a string literal or the bit format.

`<encoding>` is a String Literal with one the following values:
* `ASCII` - American Standard Code for Information Interchange (includes extended ASCII characters 128-255)
* `UTF8`, `UTF16`, `UTF32`, `UTF64` - Unicode variable-length multi-byte encoding
* `CP1252` - IBM Code Page 1252

#### D.1.2 Fixed-Length Strings

`len(<num-bytes>)` - Specifies a fixed length string format.

`<num-bytes>` is a Numeric Literal specifying the number of bytes; any remaining bytes are filled according to the DefaultStringPad setting or `pad()` attribute.

#### D.1.3 Terminated Strings

`term([<code-point>])` - Specifies that the string is character-terminated.

`<code-point>` is a Numeric Literal specifying the code point of the terminator character, in the same encoding as the string.  NOTE: Default value = `#0#`

The terminator character will be expected to immediately follow the last character in the string data.

#### D.1.4 Unterminated Strings

`unterm()` - Specifies that the string is explicitly allowed to be unterminated.

When used in conjunction with another attribute that specifies a length determination behavior (such as `term`, `len`, etc...), it is not an error for the end of the stream or a parent data container to be reached prior to what could otherwise be the end of the string.

#### D.1.5 Prefixed-Length Strings

`plen([<numeric-type>])` - Specifies that the string is encoded such that a number exists prior to the beginning of the string which contains the length of the string.

`<numeric-type>` is an integral bit format type.  NOTE: Default = `u8`

NOTE: This method of storing strings was traditionally used by programming languages such as Pascal.

#### D.1.6 Padding

`pad([<code-point>])` - Specifies padding character

`<code-point>` is a Numeric Literal specifying the code point of the padding character (overrides DefaultStringPad for this instance).

#### D.2 Field Attributes

TODO

#### D.3 Class Attributes

TODO

#### D.4 Constant Attributes

##### D.4.1 Bit Format

`type(<format-type>)` - Specifies the type for a named constant.

`<format-type>` is a `<word>` which defines the storage format.  This can be a built-in type or an alias.

NOTE: The <format-type> must be compatible with the constant's value (numeric types for Numeric Literal, string for string, etc...).
