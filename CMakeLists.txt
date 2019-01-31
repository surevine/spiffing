cmake_minimum_required(VERSION 2.8)
project(spiffing)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

set(ASN1_SOURCES ESSSecurityLabel.asn Clearance.asn SSLPrivileges.asn MissiSecurityCategories.asn acp145.asn)

add_subdirectory(deps/asn1c)

## Annoyingly, this needs to be kept up to date.
set(ASN1C_OUTPUT
ANY.c                  constr_TYPE.h              per_decoder.c
ANY.h                  per_decoder.h
asn_application.c      per_encoder.c
asn_application.h      der_encoder.c              per_encoder.h
asn_bit_data.c         der_encoder.h              PermissiveTag.c
asn_bit_data.h         EnumeratedTag.c            PermissiveTag.h
asn_codecs.h           EnumeratedTag.h            per_opentype.c
asn_codecs_prim.c      ESSPrivacyMark.c           per_opentype.h
asn_codecs_prim.h      ESSPrivacyMark.h           per_support.c
asn_internal.c         ESSSecurityLabel.c         per_support.h
asn_internal.h         ESSSecurityLabel.h         PrintableString.c
asn_ioc.h              FreeFormField.c            PrintableString.h
asn_random_fill.c      FreeFormField.h            RestrictiveTag.c
asn_random_fill.h      ID.c                       RestrictiveTag.h
asn_SEQUENCE_OF.c      ID.h                       SecurityAttribute.c
asn_SEQUENCE_OF.h      InformativeTag.c           SecurityAttribute.h
asn_SET_OF.c           InformativeTag.h           SecurityCategories.c
asn_SET_OF.h           INTEGER.c                  SecurityCategories.h
asn_system.h           INTEGER.h                  SecurityCategory.c
ber_decoder.c          INTEGER_oer.c              SECURITY-CATEGORY.c
ber_decoder.h          LrbacInfo.c                SecurityCategory.h
ber_tlv_length.c       LrbacInfo.h                SECURITY-CATEGORY.h
ber_tlv_length.h       Makefile.am.asn1convert    SecurityClassification.c
ber_tlv_tag.c          Makefile.am.libasncodec    SecurityClassification.h
ber_tlv_tag.h          MissiSecurityCategories.c  SecurityPolicyIdentifier.c
BIT_STRING.c           MissiSecurityCategories.h  SecurityPolicyIdentifier.h
BIT_STRING.h           NamedTagSet.c              SecurityTag.c
BIT_STRING_oer.c       NamedTagSet.h              SecurityTag.h
ClassList.c            NamedTagSetPrivilege.c     SecurityTagPrivilege.c
ClassList.h            NamedTagSetPrivilege.h     SecurityTagPrivilege.h
Clearance.c            NativeInteger.c            SSLPrivileges.c
Clearance.h            NativeInteger.h            SSLPrivileges.h
constraints.c          NativeInteger_oer.c        StandardSecurityLabel.c
constraints.h          OBJECT_IDENTIFIER.c        StandardSecurityLabel.h
constr_CHOICE.c        OBJECT_IDENTIFIER.h        TagSetName.c
constr_CHOICE.h        OCTET_STRING.c             TagSetName.h
constr_CHOICE_oer.c    OCTET_STRING.h             TagType7Data.c
constr_SEQUENCE.c      OCTET_STRING_oer.c         TagType7Data.h
constr_SEQUENCE.h      oer_decoder.c              UTF8String.c
constr_SEQUENCE_oer.c  oer_decoder.h              UTF8String.h
constr_SEQUENCE_OF.c   oer_encoder.c              xer_decoder.c
constr_SEQUENCE_OF.h   oer_encoder.h              xer_decoder.h
constr_SET.c           oer_support.c              xer_encoder.c
constr_SET.h           oer_support.h              xer_encoder.h
constr_SET_OF.c        OPEN_TYPE.c                xer_support.c
constr_SET_OF.h        OPEN_TYPE.h                xer_support.h
constr_SET_OF_oer.c    OPEN_TYPE_oer.c
constr_TYPE.c
)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gen-ber)
set(ASN1C_FILES)
foreach(ASN1_FILE ${ASN1_SOURCES})
    set(ASN1C_FILES ${ASN1C_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/${ASN1_FILE})
endforeach(ASN1_FILE)
add_custom_target(gen-ber
        DEPENDS ${ASN1_SOURCES} asn1c
        COMMAND asn1c -fwide-types -fcompound-names -no-gen-example ${ASN1C_FILES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gen-ber)

set(ASN_SOURCE_FILES)
foreach(ASN_SOURCE_FILE ${ASN1C_OUTPUT})
    set(ASN_SOURCE_FILES ${ASN_SOURCE_FILES} ${CMAKE_CURRENT_BINARY_DIR}/gen-ber/${ASN_SOURCE_FILE})
endforeach(ASN_SOURCE_FILE)
set_property(SOURCE ${ASN_SOURCE_FILES} PROPERTY GENERATED TRUE)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/gen-ber)
add_library(spiffing-asn ${ASN_SOURCE_FILES})
add_dependencies(spiffing-asn gen-ber)

set(SOURCE_FILES
    src/category.cc
    src/categorydata.cc
    src/categorygroup.cc
    src/catutils.cc
    src/classification.cc
    src/clearance.cc
    src/label.cc
    src/lacv.cc
    src/marking.cc
    src/spif.cc
    src/tag.cc
    src/tagset.cc include/spiffing/spiffing.h src/spiffing.cc src/equivclass.cc include/spiffing/equivclass.h src/equivcat.cc include/spiffing/equivcat.h src/constants.cc include/spiffing//constants.h include/spiffing/markings.h src/markings.cc include/spiffing/exceptions.h src/exceptions.cc)
include_directories(include)
include_directories(deps/rapidxml)
add_library(spiffing ${SOURCE_FILES})
add_dependencies(spiffing spiffing-asn)
target_link_libraries(spiffing spiffing-asn)

add_executable(spifflicator spifflicator.cc)
add_dependencies(spifflicator spiffing)
target_link_libraries(spifflicator spiffing spiffing-asn)

add_executable(test-spiffing test.cc)
add_dependencies(test-spiffing spiffing)
target_link_libraries(test-spiffing spiffing spiffing-asn)

add_executable(transpifferizer transpifferizer.cc)
target_link_libraries(transpifferizer spiffing spiffing-asn)
