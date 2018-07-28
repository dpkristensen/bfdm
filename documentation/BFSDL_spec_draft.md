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

TODO

## 3 Predefined Raw Data

TODO

## 4 BFSDL Stream Header Definition

TODO

## 5 BFSDL Stream Data Definition

TODO

## 6. Inclusion of External Definitions

TODO

## A. Change Log

1.00 Initial Release

## B. Extensions

TODO

## C. Extended Floating Point Support

TODO