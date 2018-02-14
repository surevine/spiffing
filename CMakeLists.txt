cmake_minimum_required(VERSION 2.8)
project(spiffing)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pedantic -fPIC")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

set(ASN1_SOURCES ESSSecurityLabel.asn Clearance.asn SSLPrivileges.asn MissiSecurityCategories.asn acp145.asn)

add_subdirectory(deps/asn1c)

## Annoyingly, this needs to be kept up to date.
set(ASN1C_OUTPUT
ANY.c                 der_encoder.h              per_support.c
ANY.h                 EnumeratedTag.c            per_support.h
asn_application.h     EnumeratedTag.h            PrintableString.c
asn_codecs.h          ESSPrivacyMark.c           PrintableString.h
asn_codecs_prim.c     ESSPrivacyMark.h           RestrictiveTag.c
asn_codecs_prim.h     ESSSecurityLabel.c         RestrictiveTag.h
asn_internal.h        ESSSecurityLabel.h         SecurityAttribute.c
asn_SEQUENCE_OF.c     FreeFormField.c            SecurityAttribute.h
asn_SEQUENCE_OF.h     FreeFormField.h            SecurityCategories.c
asn_SET_OF.c          ID.c                       SecurityCategories.h
asn_SET_OF.h          ID.h                       SecurityCategory.c
asn_system.h          InformativeTag.c           SECURITY-CATEGORY.c
ber_decoder.c         InformativeTag.h           SecurityCategory.h
ber_decoder.h         INTEGER.c                  SECURITY-CATEGORY.h
ber_tlv_length.c      INTEGER.h                  SecurityClassification.c
ber_tlv_length.h      LrbacInfo.c                SecurityClassification.h
ber_tlv_tag.c         LrbacInfo.h                SecurityPolicyIdentifier.c
ber_tlv_tag.h         MissiSecurityCategories.c  SecurityPolicyIdentifier.h
BIT_STRING.c          MissiSecurityCategories.h  SecurityTag.c
BIT_STRING.h          NamedTagSet.c              SecurityTag.h
ClassList.c           NamedTagSet.h              SecurityTagPrivilege.c
ClassList.h           NamedTagSetPrivilege.c     SecurityTagPrivilege.h
Clearance.c           NamedTagSetPrivilege.h     SSLPrivileges.c
Clearance.h           NativeEnumerated.c         SSLPrivileges.h
constraints.c         NativeEnumerated.h         StandardSecurityLabel.c
constraints.h         NativeInteger.c            StandardSecurityLabel.h
constr_CHOICE.c       NativeInteger.h            TagSetName.c
constr_CHOICE.h       OBJECT_IDENTIFIER.c        TagSetName.h
constr_SEQUENCE.c     OBJECT_IDENTIFIER.h        TagType7Data.c
constr_SEQUENCE.h     OCTET_STRING.c             TagType7Data.h
constr_SEQUENCE_OF.c  OCTET_STRING.h             UTF8String.c
constr_SEQUENCE_OF.h  per_decoder.c              UTF8String.h
constr_SET.c          per_decoder.h              xer_decoder.c
constr_SET.h          per_encoder.c              xer_decoder.h
constr_SET_OF.c       per_encoder.h              xer_encoder.c
constr_SET_OF.h       PermissiveTag.c            xer_encoder.h
constr_TYPE.c         PermissiveTag.h            xer_support.c
constr_TYPE.h         per_opentype.c             xer_support.h
der_encoder.c         per_opentype.h
)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gen-ber)
set(ASN1C_FILES)
foreach(ASN1_FILE ${ASN1_SOURCES})
    set(ASN1C_FILES ${ASN1C_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/${ASN1_FILE})
endforeach(ASN1_FILE)
add_custom_target(gen-ber
        DEPENDS ${ASN1_SOURCES} asn1c
        COMMAND asn1c -fwide-types ${ASN1C_FILES}
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
    src/tagset.cc include/spiffing/spiffing.h src/spiffing.cc src/equivclass.cc include/spiffing/equivclass.h src/equivcat.cc include/spiffing/equivcat.h src/constants.cc include/spiffing//constants.h include/spiffing/markings.h src/markings.cc)
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
