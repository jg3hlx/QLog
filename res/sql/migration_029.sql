ALTER TABLE ant_profiles ADD selected INTEGER;
ALTER TABLE cwkey_profiles ADD selected INTEGER;
ALTER TABLE cwshortcut_profiles ADD selected INTEGER;
ALTER TABLE rig_profiles ADD selected INTEGER;
ALTER TABLE rot_profiles ADD selected INTEGER;
ALTER TABLE rot_user_buttons_profiles ADD selected INTEGER;
ALTER TABLE station_profiles ADD selected INTEGER;
ALTER TABLE main_layout_profiles ADD selected INTEGER;

CREATE INDEX contacts_pota_idx ON contacts (pota_ref);
CREATE INDEX contacts_my_pota_idx ON contacts (my_pota_ref);
CREATE INDEX contacts_sota_idx ON contacts (sota_ref);
CREATE INDEX contacts_my_sota_idx ON contacts (my_sota_ref);
CREATE INDEX contacts_wwff_ref_idx ON contacts (wwff_ref);
CREATE INDEX contacts_my_wwff_ref_idx ON contacts (my_wwff_ref);
CREATE INDEX contacts_iota_idx ON contacts (iota);
CREATE INDEX contacts_sig_intl_idx ON contacts (sig_intl);
