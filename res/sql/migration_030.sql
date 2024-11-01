CREATE TABLE IF NOT EXISTS activity_profiles(
        profile_name TEXT PRIMARY KEY,
        config TEXT,
        selected INTEGER
);

INSERT INTO activity_profiles
SELECT profile_name, json_object(
           'activityName', profile_name,
           'profiles', json_array(
               json_object(
                   'profileType', 4,
                   'name', profile_name
               )
           )
       ) AS config,
       selected
FROM main_layout_profiles;
