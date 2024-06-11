DROP TABLE dxcc_prefixes;

CREATE TABLE "dxcc_prefixes" (
        "prefix"   TEXT NOT NULL,
        "exact"    INTEGER,
        "dxcc"     INTEGER,
        "cqz"      INTEGER,
        "ituz"     INTEGER,
        "cont"     TEXT,
        "lat"      REAL,
        "lon"      REAL,
        FOREIGN KEY("dxcc") REFERENCES "dxcc_entities"("id")
);

CREATE INDEX prefix_idx ON dxcc_prefixes(prefix);
DELETE FROM dxcc_entities;
