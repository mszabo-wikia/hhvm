// Autogenerated by Thrift for module0.thrift
//
// DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
//  @generated

package module0

import (
    "maps"

    thrift "github.com/facebook/fbthrift/thrift/lib/go/thrift/types"
    metadata "github.com/facebook/fbthrift/thrift/lib/thrift/metadata"
)

// (needed to ensure safety because of naive import list construction)
var _ = thrift.VOID
var _ = maps.Copy[map[int]int, map[int]int]
var _ = metadata.GoUnusedProtection__

// Premade Thrift types
var (
    premadeThriftType_i32 =
        &metadata.ThriftType{
            TPrimitive:
                thrift.Pointerize(metadata.ThriftPrimitiveType_THRIFT_I32_TYPE),
        }
    premadeThriftType_string =
        &metadata.ThriftType{
            TPrimitive:
                thrift.Pointerize(metadata.ThriftPrimitiveType_THRIFT_STRING_TYPE),
        }
    premadeThriftType_module0_Accessory =
        &metadata.ThriftType{
            TStruct:
                &metadata.ThriftStructType{
                    Name: "module0.Accessory",
                },
        }
    premadeThriftType_module0_PartName =
        &metadata.ThriftType{
            TStruct:
                &metadata.ThriftStructType{
                    Name: "module0.PartName",
                },
        }
)

var premadeThriftTypesMap = func() map[string]*metadata.ThriftType {
    fbthriftThriftTypesMap := make(map[string]*metadata.ThriftType)
    fbthriftThriftTypesMap["i32"] = premadeThriftType_i32
    fbthriftThriftTypesMap["string"] = premadeThriftType_string
    fbthriftThriftTypesMap["module0.Accessory"] = premadeThriftType_module0_Accessory
    fbthriftThriftTypesMap["module0.PartName"] = premadeThriftType_module0_PartName
    return fbthriftThriftTypesMap
}()

var structMetadatas = func() []*metadata.ThriftStruct {
    fbthriftResults := make([]*metadata.ThriftStruct, 0)
    func() {
        fbthriftResults = append(fbthriftResults,
            &metadata.ThriftStruct{
                Name:    "module0.Accessory",
                IsUnion: false,
                Fields:  []*metadata.ThriftField{
                    &metadata.ThriftField{
                        Id:         1,
                        Name:       "InventoryId",
                        IsOptional: false,
                        Type:       premadeThriftType_i32,
                    },
                    &metadata.ThriftField{
                        Id:         2,
                        Name:       "Name",
                        IsOptional: false,
                        Type:       premadeThriftType_string,
                    },
                },
            },
        )
    }()
    func() {
        fbthriftResults = append(fbthriftResults,
            &metadata.ThriftStruct{
                Name:    "module0.PartName",
                IsUnion: false,
                Fields:  []*metadata.ThriftField{
                    &metadata.ThriftField{
                        Id:         1,
                        Name:       "InventoryId",
                        IsOptional: false,
                        Type:       premadeThriftType_i32,
                    },
                    &metadata.ThriftField{
                        Id:         2,
                        Name:       "Name",
                        IsOptional: false,
                        Type:       premadeThriftType_string,
                    },
                },
            },
        )
    }()
    return fbthriftResults
}()

var exceptionMetadatas = func() []*metadata.ThriftException {
    fbthriftResults := make([]*metadata.ThriftException, 0)
    return fbthriftResults
}()

var enumMetadatas = func() []*metadata.ThriftEnum {
    fbthriftResults := make([]*metadata.ThriftEnum, 0)
    return fbthriftResults
}()

var serviceMetadatas = func() []*metadata.ThriftService {
    fbthriftResults := make([]*metadata.ThriftService, 0)
    return fbthriftResults
}()

// Thrift metadata for this package, as well as all of its recursive imports.
var packageThriftMetadata = func() *metadata.ThriftMetadata {
    allEnumsMap := make(map[string]*metadata.ThriftEnum)
    allStructsMap := make(map[string]*metadata.ThriftStruct)
    allExceptionsMap := make(map[string]*metadata.ThriftException)
    allServicesMap := make(map[string]*metadata.ThriftService)

    // Add enum metadatas from the current program...
    for _, enumMetadata := range enumMetadatas {
        allEnumsMap[enumMetadata.GetName()] = enumMetadata
    }
    // Add struct metadatas from the current program...
    for _, structMetadata := range structMetadatas {
        allStructsMap[structMetadata.GetName()] = structMetadata
    }
    // Add exception metadatas from the current program...
    for _, exceptionMetadata := range exceptionMetadatas {
        allExceptionsMap[exceptionMetadata.GetName()] = exceptionMetadata
    }
    // Add service metadatas from the current program...
    for _, serviceMetadata := range serviceMetadatas {
        allServicesMap[serviceMetadata.GetName()] = serviceMetadata
    }

    // Obtain Thrift metadatas from recursively included programs...
    var recursiveThriftMetadatas []*metadata.ThriftMetadata

    // ...now merge metadatas from recursively included programs.
    for _, thriftMetadata := range recursiveThriftMetadatas {
        maps.Copy(allEnumsMap, thriftMetadata.GetEnums())
        maps.Copy(allStructsMap, thriftMetadata.GetStructs())
        maps.Copy(allExceptionsMap, thriftMetadata.GetExceptions())
        maps.Copy(allServicesMap, thriftMetadata.GetServices())
    }

    return metadata.NewThriftMetadata().
        SetEnums(allEnumsMap).
        SetStructs(allStructsMap).
        SetExceptions(allExceptionsMap).
        SetServices(allServicesMap)
}()

// GetMetadataThriftType (INTERNAL USE ONLY).
// Returns metadata ThriftType for a given full type name.
func GetMetadataThriftType(fullName string) *metadata.ThriftType {
    return premadeThriftTypesMap[fullName]
}

// GetThriftMetadata returns complete Thrift metadata for current and imported packages.
func GetThriftMetadata() *metadata.ThriftMetadata {
    return packageThriftMetadata
}

// GetThriftMetadataForService returns Thrift metadata for the given service.
func GetThriftMetadataForService(scopedServiceName string) *metadata.ThriftMetadata {
    allServicesMap := packageThriftMetadata.GetServices()
    relevantServicesMap := make(map[string]*metadata.ThriftService)

    serviceMetadata := allServicesMap[scopedServiceName]
    // Visit and record all recursive parents of the target service.
    for serviceMetadata != nil {
        relevantServicesMap[serviceMetadata.GetName()] = serviceMetadata
        if serviceMetadata.IsSetParent() {
            serviceMetadata = allServicesMap[serviceMetadata.GetParent()]
        } else {
            serviceMetadata = nil
        }
    }

    return metadata.NewThriftMetadata().
        SetEnums(packageThriftMetadata.GetEnums()).
        SetStructs(packageThriftMetadata.GetStructs()).
        SetExceptions(packageThriftMetadata.GetExceptions()).
        SetServices(relevantServicesMap)
}
