// Copyright (c) Facebook, Inc. and its affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the "hack" directory of this source tree.

use hh24_types::DeclHash;
use hh24_types::FileDeclsHash;
use hh24_types::ToplevelCanonSymbolHash;
use hh24_types::ToplevelSymbolHash;
use oxidized::direct_decl_parser::ParsedFileWithHashes;
use oxidized::file_info;
use oxidized::file_info::NameType;
use relative_path::RelativePath;
use serde::Deserialize;
use serde::Serialize;

type FileDigest = String;

/// Similar to `oxidized::file_info::FileInfo`, but containing only the
/// information which is necessary to populate the naming table (i.e., omitting
/// positions). Includes hashes of individual decls.
#[derive(Clone, Debug, Hash, Serialize, Deserialize)]
pub struct FileSummary {
    pub mode: Option<file_info::Mode>,
    pub file_decls_hash: FileDeclsHash,
    pub decls: Vec<DeclSummary>,
    pub file_digest: Option<FileDigest>,
}

impl FileSummary {
    pub fn new(parsed_file: &ParsedFileWithHashes) -> Self {
        Self {
            mode: parsed_file.mode,
            file_decls_hash: parsed_file.file_decls_hash,
            decls: parsed_file
                .iter()
                .map(|(symbol, decl, hash, sort_text)| DeclSummary {
                    name_type: decl.kind(),
                    symbol: symbol.clone(),
                    hash: hash.clone(),
                    sort_text: sort_text.clone(),
                })
                .collect(),
            file_digest: None,
        }
    }

    pub fn example_symbol(&self) -> Option<String> {
        self.decls.first().map(|decl| &decl.symbol).cloned()
    }

    pub fn decl_hashes(&self) -> impl Iterator<Item = (ToplevelSymbolHash, DeclHash)> + '_ {
        self.decls
            .iter()
            .map(|decl| (decl.symbol_hash(), decl.hash))
    }

    pub fn symbols_of_kind(&self, kind: NameType) -> impl Iterator<Item = &DeclSummary> + '_ {
        self.decls.iter().filter(move |decl| decl.name_type == kind)
    }

    pub fn classes(&self) -> impl Iterator<Item = &DeclSummary> + '_ {
        self.symbols_of_kind(NameType::Class)
    }

    pub fn funs(&self) -> impl Iterator<Item = &DeclSummary> + '_ {
        self.symbols_of_kind(NameType::Fun)
    }

    pub fn consts(&self) -> impl Iterator<Item = &DeclSummary> + '_ {
        self.symbols_of_kind(NameType::Const)
    }

    pub fn typedefs(&self) -> impl Iterator<Item = &DeclSummary> + '_ {
        self.symbols_of_kind(NameType::Typedef)
    }

    pub fn modules(&self) -> impl Iterator<Item = &DeclSummary> + '_ {
        self.symbols_of_kind(NameType::Module)
    }
}

#[derive(Clone, Debug, Hash, Serialize, Deserialize)]
pub struct DeclSummary {
    pub name_type: file_info::NameType,
    pub symbol: String,
    pub hash: DeclHash,
    pub sort_text: Option<String>,
}

impl DeclSummary {
    pub fn symbol_hash(&self) -> ToplevelSymbolHash {
        ToplevelSymbolHash::new(self.name_type, &self.symbol)
    }
}

/// This is what each row looks like in NAMING_SYMBOLS and NAMING_SYMBOLS_OVERFLOW,
/// i.e. the reverse naming table. When this struct is used for reading from the
/// database, we generally join NAMING_FILE_INFO to populate 'path' since there's very
/// little you can do with a row without turning file_info_id into a pathname;
/// when used for writing, we generally ignore 'path'.
/// This structure is a bit like DeclSummary. The difference is that SymbolRow
/// can be reconstructed from out of the reverse-naming-table (hence can be used
/// by algorithms that want to retrieve then write-back stuff from the table),
/// while DeclSummary can't.
#[derive(Clone, Debug)]
pub struct SymbolRow {
    pub hash: ToplevelSymbolHash,
    pub canon_hash: ToplevelCanonSymbolHash,
    pub decl_hash: DeclHash,
    pub kind: NameType,
    pub file_info_id: crate::FileInfoId,
    pub path: RelativePath,
}

#[derive(Clone, Debug)]
pub struct SymbolRowNew {
    pub hash: ToplevelSymbolHash,
    pub canon_hash: ToplevelCanonSymbolHash,
    pub decl_hash: DeclHash,
    pub kind: NameType,
    pub name: String,
    pub sort_text: String,
    pub path: RelativePath,
    pub file_info_id: crate::FileInfoId,
}
