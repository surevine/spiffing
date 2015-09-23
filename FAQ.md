Spiffing FAQ
============

## Why "Spiffing"?

Because it deals with SPIFs, and the other Security Information Objects associated with them - Labels and Clearances.

## No, I mean, why are you writing this?

Ah! Because these facilities are often desirable, or even required, for government and military systems, and without a
liberally licensed, open-source library, it drives the cost of such software up very high and additionally restricts
who is able to offer such systems.

Since I both pay tax, and work within this industry (via Surevine), I've a vested interest in both.

## What's a SPIF?

A SPIF is a Security Policy Information File. It describes things like what classifications exist, what categories there
are, and so on. It also describes what category tags are legal with, or required by, which others.

## What's a Label?

Labels are similarly like the films.

They contain one classification, and a set of tags further describing who can see the information, and these tags are
validated according to the rules in the SPIF. This validation is crucial, because without it, it might be possible to
craft a label which incorrectly passed the ACDF despite appearing to be legitimate.

Labels are encoded, typically as one of the ASN.1 or XML formats.

## I thought labels are text?

No; that's a "Display Marking", and you can make one of those from a label. In some cases, you can work backwards, but
it's a heuristic mapping. The SPIF tells Spiffing how to go from the label to a marking. 

## What's a Clearance?

A clearance is, more or less, like the films. People are cleared to a certain level, and they're also cleared to handle
certain kinds of information, such as nuclear information, or personel files. Things are also cleared - a system might
be deployed only for more sensitive information, and is therefore not granted clearance for less sensitive information.

This means that a Clearance has a set of classifications; it's not just one. If you're cleared to see "up to" TLP:RED,
your clearance would therefore include TLP:AMBER and TLP:GREEN explicitly.

Clearances are also used to express what information can legally pass through certain channels, because that channel
isn't encrypted, goes to a foreign power, or whatever.

Some systems prefer to use a label as a clearance; this is problematic because a clearance isn't validated but a label
is (or should be), and label used in this way has to indicate a clearance "up to" a particular classification.

## Erm, you mentioned an ACDF?

Access Control Decision Function. The function which decides whether a label is "dominated by" a clearance.

## What's this about syntaxes and formats?

The main formats in use are:

* X.841 - an ASN.1 format found in ESS (Enhanced Security Services for S/MIME),
* EDH - Enterprise Data Headers, and really Intelligence Community Enterprise Data Headers, an XML format for general metadata, and
* NXL - NATO XML Labelling, which is itself classified.

Spiffing currently speaks only the X.841 format (and a debugging XML format).

X.841 leaves the ASN.1 syntax for Category data up to the implementation, and there are two syntaxes in use:

* ACP-145(A) - NATO's format, and
* SDN.801c - the NSA/NIST format, formally specified in SDN.702c

Spiffing speaks both.

X.841 and ACP-145(A) are both publicly available. SDN.801c and SDN.702c are not classified, but may not be freely
distributed.

## You keep mentioning Categories and Tags...?

Categories are additional divisions used for further control of the information beyond simple classifications. For
example, "Releasable To" is a Category. Within each Category there are one or more Tags - in this example, countries.
Categories are sometimes known as Groups.

Categories come in three flavours:
 
* Restrictive (also known as "And Groups" or "Closed Groups"), where the clearance must have all the Tags the label has.
* Permissive (also known as "Or Groups" or "Open Groups"), where the clearance must have any of the Tags the label has.
* Informative (sometimes known as Caveats), which only occur in the label, and are for people, not computers, to interpret.

## How come there are no decent examples?

In general, real security policies are themselves classified, and therefore so are the SPIFs that represent them. I
could come up with something reasonable, perhaps based around the UK Government Classification Scheme, which is public,
but including the names of real classifications can cause problems with people's content filters, so I avoid it.

The lack of available policies is a problem for testing, as you can imagine...

## If you ruled the world, what could you do to make this easier?

First off, declassify and publish the relevant documents! Certainly it'd be tremendously useful to have SDN.801c etc
public, and I don't understand why it isn't. Similarly, I have no idea why the NXL format isn't available.

Secondly, a set of test vectors would be trivial to produce against a SPIF encapsulating the UK GCS, for example, and
would enormously simplify testing.
