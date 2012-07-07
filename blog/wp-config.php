<?php
/**
 * The base configurations of the WordPress.
 *
 * This file has the following configurations: MySQL settings, Table Prefix,
 * Secret Keys, WordPress Language, and ABSPATH. You can find more information
 * by visiting {@link http://codex.wordpress.org/Editing_wp-config.php Editing
 * wp-config.php} Codex page. You can get the MySQL settings from your web host.
 *
 * This file is used by the wp-config.php creation script during the
 * installation. You don't have to use the web site, you can just copy this file
 * to "wp-config.php" and fill in the values.
 *
 * @package WordPress
 */

// ** MySQL settings - You can get this info from your web host ** //
/** The name of the database for WordPress */
define('DB_NAME', 'wordpress');

/** MySQL database username */
define('DB_USER', 'wp_admin');

/** MySQL database password */
define('DB_PASSWORD', 'Mini$ter');

/** MySQL hostname */
define('DB_HOST', 'localhost');

/** Database Charset to use in creating database tables. */
define('DB_CHARSET', 'utf8');

/** The Database Collate type. Don't change this if in doubt. */
define('DB_COLLATE', '');

/**#@+
 * Authentication Unique Keys and Salts.
 *
 * Change these to different unique phrases!
 * You can generate these using the {@link https://api.wordpress.org/secret-key/1.1/salt/ WordPress.org secret-key service}
 * You can change these at any point in time to invalidate all existing cookies. This will force all users to have to log in again.
 *
 * @since 2.6.0
 */
define('AUTH_KEY',         'wVHL^E_|Zn!:kj/LA>CQL ;Kvv/ncwX+XvCQX:Kipi+fs`c|TDq_f;2|t#.se[wc');
define('SECURE_AUTH_KEY',  '1u&z^EjM>2 >g`|r+Ua28g-DyhkP[=+&_/C%6J}Q-<*RK:oh<f3v9r%=c^*p|+1u');
define('LOGGED_IN_KEY',    '8.*tyxQ-I[JiUy!dyLs_Zv,Wx0rm?T+lXknqEmtFmH^tr[C+$[[&e1S<.J&|W?[Y');
define('NONCE_KEY',        ';-J!W+:v({FVodipazguc+u+J(zEtezVC!<J~Un:e?xAhpXWAG%LZg|p#I,I:YQ$');
define('AUTH_SALT',        'cHFXGrsL*{===RpM8ptejlRf-Fb8)m4QvQ}8hQT9(O`>^[;FvC? eY[x3s7XkEH#');
define('SECURE_AUTH_SALT', 'E/`gEM-b*.Q;2zrJ|Q _*gO0OA[Y~!>zcfgB=8^W*-8uWbXZcN(4$$/$-Ra[up+9');
define('LOGGED_IN_SALT',   '`xIt-1-@?Ew:i+dpH.UBI4>Wv]Ov%DNcEt)`FeQXt].plrC3Gt,=PL:QT>w{B9m/');
define('NONCE_SALT',       'W~ -1UycNa[TWS !|)sjHj:U#OQRL%]@>mf-+O%:SA~9iOD-@X|%k:%.~bjP&+(q');

/**#@-*/

/**
 * WordPress Database Table prefix.
 *
 * You can have multiple installations in one database if you give each a unique
 * prefix. Only numbers, letters, and underscores please!
 */
$table_prefix  = 'wp_';

/**
 * WordPress Localized Language, defaults to English.
 *
 * Change this to localize WordPress. A corresponding MO file for the chosen
 * language must be installed to wp-content/languages. For example, install
 * de_DE.mo to wp-content/languages and set WPLANG to 'de_DE' to enable German
 * language support.
 */
define('WPLANG', '');

/**
 * For developers: WordPress debugging mode.
 *
 * Change this to true to enable the display of notices during development.
 * It is strongly recommended that plugin and theme developers use WP_DEBUG
 * in their development environments.
 */
define('WP_DEBUG', false);

/* That's all, stop editing! Happy blogging. */

/** Absolute path to the WordPress directory. */
if ( !defined('ABSPATH') )
	define('ABSPATH', dirname(__FILE__) . '/');

/** Sets up WordPress vars and included files. */
require_once(ABSPATH . 'wp-settings.php');
