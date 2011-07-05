#ifndef __CPROTO__
/* client/animations.c */
void read_anims();
void anims_reset();

/* client/client.c */
void DoClient();
void SockList_Init(SockList *sl);
void SockList_AddChar(SockList *sl, char c);
void SockList_AddShort(SockList *sl, uint16 data);
void SockList_AddInt(SockList *sl, uint32 data);
void SockList_AddString(SockList *sl, char *data);
void SockList_AddStringTerminated(SockList *sl, char *data);
int GetInt_String(const unsigned char *data);
sint64 GetInt64_String(const unsigned char *data);
short GetShort_String(const unsigned char *data);
char *GetString_String(uint8 *data, int *pos, char *dest, size_t dest_size);
int cs_write_string(char *buf, size_t len);
void check_animation_status(int anum);

/* client/commands.c */
void BookCmd(unsigned char *data, int len);
void SetupCmd(char *buf, int len);
void AddMeFail(unsigned char *data, int len);
void AddMeSuccess(unsigned char *data, int len);
void AnimCmd(unsigned char *data, int len);
void ImageCmd(unsigned char *data, int len);
void SkillRdyCmd(char *data, int len);
void DrawInfoCmd(unsigned char *data);
void DrawInfoCmd2(unsigned char *data, int len);
void TargetObject(unsigned char *data, int len);
void StatsCmd(unsigned char *data, int len);
void PreParseInfoStat(char *cmd);
void handle_query(char *data);
void send_reply(char *text);
void PlayerCmd(unsigned char *data, int len);
void ItemXCmd(unsigned char *data, int len);
void ItemYCmd(unsigned char *data, int len);
void UpdateItemCmd(unsigned char *data, int len);
void DeleteItem(unsigned char *data, int len);
void DeleteInventory(unsigned char *data);
void MapStatsCmd(unsigned char *data, int len);
void Map2Cmd(unsigned char *data, int len);
void MagicMapCmd(unsigned char *data, int len);
void VersionCmd(char *data);
void SendVersion();
void RequestFile(int index);
void SendAddMe();
void NewCharCmd();
void DataCmd(unsigned char *data, int len);
void ShopCmd(unsigned char *data, int len);
void QuestListCmd(unsigned char *data, int len);
void ReadyCmd(unsigned char *data, int len);

/* client/curl.c */
int curl_connect(void *c_data);
curl_data *curl_data_new(const char *url);
curl_data *curl_download_start(const char *url);
sint8 curl_download_finished(curl_data *data);
void curl_data_free(curl_data *data);
void curl_init();
void curl_deinit();

/* client/ignore.c */
void ignore_list_clear();
void ignore_list_load();
int ignore_check(const char *name, const char *type);
void ignore_command(const char *cmd);

/* client/image.c */
bmap_struct *bmap_find(const char *name);
void bmap_add(bmap_struct *bmap);
void read_bmaps_p0();
void read_bmaps();
void finish_face_cmd(int pnum, uint32 checksum, char *face);
int request_face(int pnum);
int get_bmap_id(char *name);
void blit_face(int id, int x, int y);

/* client/item.c */
void objects_free(object *op);
object *object_find_object_inv(object *op, sint32 tag);
object *object_find_object(object *op, sint32 tag);
object *object_find(sint32 tag);
void object_remove(object *op);
void object_remove_inventory(object *op);
object *object_create(object *env, sint32 tag, int bflag);
void object_set_values(object *op, const char *name, sint32 weight, uint16 face, int flags, uint16 anim, uint16 animspeed, sint32 nrof, uint8 itype, uint8 stype, uint8 qual, uint8 cond, uint8 skill, uint8 level, uint8 dir);
void toggle_locked(object *op);
void object_send_mark(object *op);
void ready_object(object *op);
void objects_init();
void update_object(int tag, int loc, const char *name, int weight, int face, int flags, int anim, int animspeed, int nrof, uint8 itype, uint8 stype, uint8 qual, uint8 cond, uint8 skill, uint8 level, uint8 direction, int bflag);
void animate_objects();

/* client/main.c */
void free_bitmaps();
void list_vid_modes();
int main(int argc, char *argv[]);

/* client/menu.c */
int client_command_check(const char *cmd);
void send_command_check(const char *cmd);
void blt_inventory_face_from_tag(int tag, int x, int y);
void show_menu();
void blt_window_slider(_Sprite *slider, int maxlen, int winlen, int startoff, int len, int x, int y);

/* client/metaserver.c */
void metaserver_init();
void metaserver_disable();
server_struct *server_get_id(size_t num);
size_t server_get_count();
int ms_connecting(int val);
void metaserver_clear_data();
void metaserver_add(const char *ip, int port, const char *name, int player, const char *version, const char *desc);
int metaserver_thread(void *dummy);
void metaserver_get_servers();

/* client/misc.c */
unsigned long isqrt(unsigned long n);
size_t split_string(char *str, char *array[], size_t array_size, char sep);
void *reallocz(void *ptr, size_t old_size, size_t new_size);
void convert_newline(char *str);
void browser_open(const char *url);
int rndm(int min, int max);

/* client/player.c */
void clear_player();
void new_player(long tag, long weight, short face);
void client_send_apply(int tag);
void client_send_examine(int tag);
void client_send_move(int loc, int tag, int nrof);
void send_command(const char *command);
void CompleteCmd(unsigned char *data, int len);
void set_weight_limit(uint32 wlim);
void init_player_data();
void widget_player_data_event(widgetdata *widget, int x, int y);
void widget_show_player_data(widgetdata *widget);
void widget_player_stats(widgetdata *widget);
void widget_skillgroups(widgetdata *widget);
void widget_show_player_doll_event();
void widget_show_player_doll(widgetdata *widget);
void widget_show_main_lvl(widgetdata *widget);
void widget_show_skill_exp(widgetdata *widget);
void widget_show_regeneration(widgetdata *widget);
void widget_show_container(widgetdata *widget);
void widget_highlight_menu(widgetdata *widget);
void widget_menu_event(widgetdata *widget, int x, int y);
void widget_menuitem_event(widgetdata *widget, int x, int y, void (*menu_func_ptr)(widgetdata *, int, int));
void widget_show_label(widgetdata *widget);
void widget_show_bitmap(widgetdata *widget);
int gender_to_id(const char *gender);

/* client/scripts.c */
void script_load(const char *cparams);
void script_list();
void script_process();
int script_trigger_event(const char *cmd, const uint8 *data, const int data_len, const enum CmdFormat format);
void script_send(const char *params);
void script_killall();
void script_autoload();
void script_unload(const char *params);

/* client/server_files.c */
void server_files_init();
void server_files_load(int post_load);
FILE *server_file_open(size_t id);
void server_file_save(size_t id, unsigned char *data, size_t len);
int server_files_updating();
void server_files_setup_add(char *buf, size_t buf_size);
int server_files_parse_setup(const char *cmd, const char *param);
void server_files_clear_update();

/* client/server_settings.c */
void server_settings_init();
void server_settings_deinit();

/* client/socket.c */
void command_buffer_free(command_buffer *buf);
int send_command_binary(uint8 cmd, uint8 *body, unsigned int len);
int send_socklist(SockList msg);
command_buffer *get_next_input_command();
void socket_thread_start();
void socket_thread_stop();
int handle_socket_shutdown();
int socket_get_error();
int socket_close(struct ClientSocket *csock);
int socket_initialize();
void socket_deinitialize();
int socket_open(struct ClientSocket *csock, char *host, int port);

/* client/sound.c */
void sound_init();
void sound_deinit();
void sound_play_effect(const char *filename, int volume);
void sound_start_bg_music(const char *filename, int volume, int loop);
void sound_stop_bg_music();
void update_map_bg_music(const char *bg_music);
void sound_update_volume();
const char *sound_get_bg_music();
const char *sound_get_bg_music_basename();
uint8 sound_map_background(int new);
void SoundCmd(uint8 *data, int len);

/* client/sprite.c */
void sprite_init_system();
_Sprite *sprite_load_file(char *fname, uint32 flags);
_Sprite *sprite_tryload_file(char *fname, uint32 flag, SDL_RWops *rwop);
void sprite_free_sprite(_Sprite *sprite);
void sprite_blt(_Sprite *sprite, int x, int y, SDL_Rect *box, _BLTFX *bltfx);
void sprite_blt_map(_Sprite *sprite, int x, int y, SDL_Rect *box, _BLTFX *bltfx, uint32 stretch, sint16 zoom, sint16 rotate);
Uint32 getpixel(SDL_Surface *surface, int x, int y);
void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
struct _anim *add_anim(int type, int mapx, int mapy, int value);
void remove_anim(struct _anim *anim);
void play_anims();
int sprite_collision(int x1, int y1, int x2, int y2, _Sprite *sprite1, _Sprite *sprite2);
void surface_pan(SDL_Surface *surface, SDL_Rect *box);
void draw_frame(SDL_Surface *surface, int x, int y, int w, int h);
void border_create(SDL_Surface *surface, int x, int y, int w, int h, int color, int size);
void border_create_color(SDL_Surface *surface, SDL_Rect *coords, const char *color_notation);

/* client/tilestretcher.c */
int add_color_to_surface(SDL_Surface *dest, Uint8 red, Uint8 green, Uint8 blue);
int copy_pixel_to_pixel(SDL_Surface *src, SDL_Surface *dest, int x1, int y1, int x2, int y2, float brightness);
int copy_vertical_line(SDL_Surface *src, SDL_Surface *dest, int src_x, int src_sy, int src_ey, int dest_x, int dest_sy, int dest_ey, float brightness, int extra);
SDL_Surface *tile_stretch(SDL_Surface *src, int n, int e, int s, int w);

/* client/updates.c */
void cmd_request_update(unsigned char *data, int len);
int file_updates_finished();
void file_updates_parse();

/* client/upgrader.c */
void upgrade_do(const char *source_dir);
void upgrader_init();

/* client/video.c */
int video_get_bpp();
int video_set_size();
uint32 get_video_flags();
int video_fullscreen_toggle(SDL_Surface **surface, uint32 *flags);

/* client/wrapper.c */
void LOG(LogLevel logLevel, char *format, ...) __attribute__((format(printf, 2, 3)));
void system_start();
void system_end();
char *get_word_from_string(char *str, int *pos);
void copy_file(const char *filename, const char *filename_out);
const char *get_config_dir();
char *file_path(const char *fname, const char *mode);
FILE *fopen_wrapper(const char *fname, const char *mode);
SDL_Surface *IMG_Load_wrapper(const char *file);

/* events/console.c */
void mouse_InputNumber();

/* events/event.c */
uint8 key_is_pressed(SDLKey key);
int draggingInvItem(int src);
void resize_window(int width, int height);
int Event_PollInputDevice();

/* events/keys.c */
int key_event(SDL_KeyboardEvent *key);
int event_poll_key(SDL_Event *event);
void cursor_keys(int num);
void key_repeat();
void check_menu_keys(int menu, int key);

/* events/macro.c */
void init_keys();
void reset_keys();
int check_menu_macros(char *text);
int check_keys_menu_status(int key);
void process_macro(_keymap macro);
void check_keys(int key);
int process_macro_keys(int id, int value);
void read_keybind_file(char *fname);
void save_keybind_file(char *fname);

/* events/move.c */
void move_keys(int num);
int dir_from_tile_coords(int tx, int ty);

/* gui/book.c */
void book_name_change(const char *name, size_t len);
void book_load(const char *data, int len);
void book_show();
void book_handle_key(SDLKey key);
void book_handle_event(SDL_Event *event);
void book_redraw();

/* gui/effects.c */
void effects_init();
void effects_deinit();
void effects_reinit();
void effect_sprites_free(effect_struct *effect);
void effect_free(effect_struct *effect);
void effect_sprite_def_free(effect_sprite_def *sprite_def);
void effect_sprite_free(effect_sprite *sprite);
void effect_sprite_remove(effect_sprite *sprite);
void effect_sprites_play();
int effect_start(const char *name);
void effect_debug(const char *type);
void effect_stop();
uint8 effect_has_overlay();
void effect_scale(_Sprite *sprite);

/* gui/fps.c */
void fps_init();
void fps_do();
void widget_show_fps(widgetdata *widget);

/* gui/help.c */
void free_help_files();
void read_help_files();
void show_help(const char *helpname);

/* gui/input.c */
void widget_number_event(widgetdata *widget, int x, int y);
void widget_show_console(widgetdata *widget);
void widget_show_number(widgetdata *widget);
void do_number();
void do_keybind_input();
void do_console();

/* gui/inventory.c */
void inventory_filter_set(uint64 filter);
void inventory_filter_toggle(uint64 filter);
int get_inventory_data(object *op, int *ctag, int *slot, int *start, int *count, int wxlen, int wylen);
void widget_inventory_event(widgetdata *widget, int x, int y, SDL_Event event);
void widget_show_inventory_window(widgetdata *widget);
void widget_below_window_event(widgetdata *widget, int x, int y, int MEvent);
void widget_show_below_window(widgetdata *widget);
int blt_inv_item_centered(object *tmp, int x, int y);
void blt_inv_item(object *tmp, int x, int y);

/* gui/keybind.c */
void show_keybind();

/* gui/main.c */
void show_meta_server();

/* gui/map.c */
void load_mapdef_dat();
void widget_show_mapname(widgetdata *widget);
void clear_map();
void display_mapscroll(int dx, int dy);
void update_map_name(const char *name);
void update_map_weather(const char *weather);
void init_map_data(int xl, int yl, int px, int py);
void align_tile_stretch(int x, int y);
void adjust_tile_stretch();
void map_set_data(int x, int y, int layer, sint16 face, uint8 quick_pos, uint8 obj_flags, const char *name, const char *name_color, sint16 height, uint8 probe, sint16 zoom, sint16 align, uint8 draw_double, uint8 alpha, sint16 rotate, uint8 infravision);
void map_clear_cell(int x, int y);
void map_set_darkness(int x, int y, uint8 darkness);
void map_draw_map();
void map_draw_one(int x, int y, _Sprite *sprite);
void widget_map_mevent(widgetdata *widget, SDL_Event *event);
void widget_map_render(widgetdata *widget);
int mouse_to_tile_coords(int mx, int my, int *tx, int *ty);

/* gui/menu_buttons.c */
void widget_menubuttons(widgetdata *widget);
void widget_menubuttons_event(widgetdata *widget, SDL_Event *event);

/* gui/mplayer.c */
void widget_show_mplayer(widgetdata *widget);
void widget_mplayer_background(widgetdata *widget);
void widget_mplayer_deinit(widgetdata *widget);
void widget_mplayer_mevent(widgetdata *widget, SDL_Event *event);
void mplayer_now_playing();

/* gui/party.c */
void widget_party_background(widgetdata *widget);
void widget_party_render(widgetdata *widget);
void widget_party_mevent(widgetdata *widget, SDL_Event *event);
void PartyCmd(unsigned char *data, int len);

/* gui/protections.c */
void widget_show_resist(widgetdata *widget);

/* gui/quickslots.c */
void quickslots_init();
void quickslots_handle_key(SDL_KeyboardEvent *key, int slot);
int get_quickslot(int x, int y);
void show_quickslots(int x, int y, int vertical_quickslot);
void widget_quickslots(widgetdata *widget);
void widget_quickslots_mouse_event(widgetdata *widget, SDL_Event *event);
void QuickSlotCmd(unsigned char *data, int len);

/* gui/range.c */
void widget_range_event(widgetdata *widget, int x, int y, SDL_Event event, int MEvent);
void widget_show_range(widgetdata *widget);

/* gui/region_map.c */
void RegionMapCmd(uint8 *data, int len);
void region_map_clear();
void region_map_handle_key(SDLKey key);
void region_map_handle_event(SDL_Event *event);
void region_map_show();

/* gui/settings.c */
void *setting_get(setting_struct *setting);
const char *setting_get_str(int cat, int setting);
sint64 setting_get_int(int cat, int setting);
void setting_apply_change(int cat, int setting);
void setting_set_int(int cat, int setting, sint64 val);
int setting_is_text(setting_struct *setting);
void setting_load_value(setting_struct *setting, const char *str);
void settings_apply();
void settings_init();
void settings_save();
sint64 category_from_name(const char *name);
sint64 setting_from_name(const char *name);
void settings_load();
void settings_deinit();
void settings_open();

/* gui/skills.c */
void widget_skills_render(widgetdata *widget);
void widget_skills_mevent(widgetdata *widget, SDL_Event *event);
int skill_find(const char *name, size_t *type, size_t *id);
int skill_find_type_selected(const char *name, size_t *id);
skill_entry_struct *skill_get(size_t type, size_t id);
void skills_init();
void skills_reload();
void SkilllistCmd(char *data);

/* gui/spells.c */
void widget_spells_render(widgetdata *widget);
void widget_spells_mevent(widgetdata *widget, SDL_Event *event);
int spell_find(const char *name, size_t *spell_path, size_t *spell_id);
int spell_find_path_selected(const char *name, size_t *spell_id);
spell_entry_struct *spell_get(size_t spell_path, size_t spell_id);
void spells_init();
void spells_reload();
void SpelllistCmd(char *data);

/* gui/target.c */
void widget_event_target(widgetdata *widget, int x, int y);
void widget_show_target(widgetdata *widget);

/* gui/textwin.c */
void textwin_init();
void textwin_scroll_adjust(widgetdata *widget);
void textwin_readjust(widgetdata *widget);
void draw_info_flags(const char *color, int flags, const char *str);
void draw_info_format(const char *color, char *format, ...) __attribute__((format(printf, 2, 3)));
void draw_info(const char *color, const char *str);
void textwin_handle_copy();
void textwin_show(int x, int y, int w, int h);
void widget_textwin_show(widgetdata *widget);
void textwin_event(widgetdata *widget, SDL_Event *event);
void menu_textwin_clear(widgetdata *widget, int x, int y);
void menu_textwin_font_inc(widgetdata *widget, int x, int y);
void menu_textwin_font_dec(widgetdata *widget, int x, int y);

/* toolkit/button.c */
int button_show(int bitmap_id, int bitmap_id_over, int bitmap_id_clicked, int x, int y, const char *text, int font, const char *color, const char *color_shadow, const char *color_over, const char *color_over_shadow, uint64 flags);
void button_create(button_struct *button);
void button_render(button_struct *button, const char *text);
int button_event(button_struct *button, SDL_Event *event);
void button_tooltip(button_struct *button, int font, const char *text);

/* toolkit/clipboard.c */
int clipboard_init();
int clipboard_set(const char *str);
char *clipboard_get();

/* toolkit/list.c */
list_struct *list_get_focused();
void list_set_focus(list_struct *list);
void list_set_parent(list_struct *list, int px, int py);
list_struct *list_create(uint32 id, uint32 max_rows, uint32 cols, int spacing);
void list_add(list_struct *list, uint32 row, uint32 col, const char *str);
void list_remove_row(list_struct *list, uint32 row);
void list_set_column(list_struct *list, uint32 col, int width, int spacing, const char *name, int centered);
void list_set_font(list_struct *list, int font);
void list_scrollbar_enable(list_struct *list);
void list_show(list_struct *list, int x, int y);
void list_clear(list_struct *list);
void list_remove(list_struct *list);
void list_remove_all();
int list_handle_keyboard(list_struct *list, SDL_KeyboardEvent *event);
int lists_handle_keyboard(SDL_KeyboardEvent *event);
int list_handle_mouse(list_struct *list, int mx, int my, SDL_Event *event);
int lists_handle_mouse(int mx, int my, SDL_Event *event);
list_struct *list_exists(uint32 id);
void list_sort(list_struct *list, int type);
void list_clear_rows(list_struct *list);
int list_set_selected(list_struct *list, const char *str, uint32 col);

/* toolkit/popup.c */
popup_struct *popup_create(int bitmap_id);
void popup_destroy_visible();
int popup_overlay_need_update(popup_struct *popup);
void popup_draw();
int popup_handle_event(SDL_Event *event);
popup_struct *popup_get_visible();

/* toolkit/range_buttons.c */
int range_buttons_show(int x, int y, int *val, int advance);

/* toolkit/scroll_buttons.c */
void scroll_buttons_show(SDL_Surface *surface, int x, int y, int *pos, int max_pos, int advance, SDL_Rect *box);

/* toolkit/SDL_gfx.c */
int fastPixelColorNolock(SDL_Surface *dst, Sint16 x, Sint16 y, Uint32 color);
int fastPixelColorNolockNoclip(SDL_Surface *dst, Sint16 x, Sint16 y, Uint32 color);
int fastPixelColor(SDL_Surface *dst, Sint16 x, Sint16 y, Uint32 color);
int fastPixelRGBA(SDL_Surface *dst, Sint16 x, Sint16 y, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int fastPixelRGBANolock(SDL_Surface *dst, Sint16 x, Sint16 y, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int _putPixelAlpha(SDL_Surface *dst, Sint16 x, Sint16 y, Uint32 color, Uint8 alpha);
int pixelColor(SDL_Surface *dst, Sint16 x, Sint16 y, Uint32 color);
int pixelColorNolock(SDL_Surface *dst, Sint16 x, Sint16 y, Uint32 color);
int _filledRectAlpha(SDL_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color, Uint8 alpha);
int filledRectAlpha(SDL_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color);
int _HLineAlpha(SDL_Surface *dst, Sint16 x1, Sint16 x2, Sint16 y, Uint32 color);
int _VLineAlpha(SDL_Surface *dst, Sint16 x, Sint16 y1, Sint16 y2, Uint32 color);
int pixelColorWeight(SDL_Surface *dst, Sint16 x, Sint16 y, Uint32 color, Uint32 weight);
int pixelColorWeightNolock(SDL_Surface *dst, Sint16 x, Sint16 y, Uint32 color, Uint32 weight);
int pixelRGBA(SDL_Surface *dst, Sint16 x, Sint16 y, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int hlineColorStore(SDL_Surface *dst, Sint16 x1, Sint16 x2, Sint16 y, Uint32 color);
int hlineRGBAStore(SDL_Surface *dst, Sint16 x1, Sint16 x2, Sint16 y, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int hlineColor(SDL_Surface *dst, Sint16 x1, Sint16 x2, Sint16 y, Uint32 color);
int hlineRGBA(SDL_Surface *dst, Sint16 x1, Sint16 x2, Sint16 y, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int vlineColor(SDL_Surface *dst, Sint16 x, Sint16 y1, Sint16 y2, Uint32 color);
int vlineRGBA(SDL_Surface *dst, Sint16 x, Sint16 y1, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int rectangleColor(SDL_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color);
int rectangleRGBA(SDL_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int roundedRectangleColor(SDL_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 rad, Uint32 color);
int roundedRectangleRGBA(SDL_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int roundedBoxColor(SDL_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 rad, Uint32 color);
int roundedBoxRGBA(SDL_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int boxColor(SDL_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color);
int boxRGBA(SDL_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int lineColor(SDL_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color);
int lineRGBA(SDL_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int _aalineColor(SDL_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color, int draw_endpoint);
int aalineColor(SDL_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint32 color);
int aalineRGBA(SDL_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int circleColor(SDL_Surface *dst, Sint16 x, Sint16 y, Sint16 rad, Uint32 color);
int circleRGBA(SDL_Surface *dst, Sint16 x, Sint16 y, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int arcColor(SDL_Surface *dst, Sint16 x, Sint16 y, Sint16 rad, Sint16 start, Sint16 end, Uint32 color);
int arcRGBA(SDL_Surface *dst, Sint16 x, Sint16 y, Sint16 rad, Sint16 start, Sint16 end, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int aacircleColor(SDL_Surface *dst, Sint16 x, Sint16 y, Sint16 rad, Uint32 color);
int aacircleRGBA(SDL_Surface *dst, Sint16 x, Sint16 y, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int filledCircleColor(SDL_Surface *dst, Sint16 x, Sint16 y, Sint16 rad, Uint32 color);
int filledCircleRGBA(SDL_Surface *dst, Sint16 x, Sint16 y, Sint16 rad, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int ellipseColor(SDL_Surface *dst, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint32 color);
int ellipseRGBA(SDL_Surface *dst, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int aaellipseColor(SDL_Surface *dst, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint32 color);
int aaellipseRGBA(SDL_Surface *dst, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int filledEllipseColor(SDL_Surface *dst, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint32 color);
int filledEllipseRGBA(SDL_Surface *dst, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int _pieColor(SDL_Surface *dst, Sint16 x, Sint16 y, Sint16 rad, Sint16 start, Sint16 end, Uint32 color, Uint8 filled);
int pieColor(SDL_Surface *dst, Sint16 x, Sint16 y, Sint16 rad, Sint16 start, Sint16 end, Uint32 color);
int pieRGBA(SDL_Surface *dst, Sint16 x, Sint16 y, Sint16 rad, Sint16 start, Sint16 end, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int filledPieColor(SDL_Surface *dst, Sint16 x, Sint16 y, Sint16 rad, Sint16 start, Sint16 end, Uint32 color);
int filledPieRGBA(SDL_Surface *dst, Sint16 x, Sint16 y, Sint16 rad, Sint16 start, Sint16 end, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int trigonColor(SDL_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 x3, Sint16 y3, Uint32 color);
int trigonRGBA(SDL_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 x3, Sint16 y3, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int aatrigonColor(SDL_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 x3, Sint16 y3, Uint32 color);
int aatrigonRGBA(SDL_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 x3, Sint16 y3, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int filledTrigonColor(SDL_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 x3, Sint16 y3, Uint32 color);
int filledTrigonRGBA(SDL_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 x3, Sint16 y3, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int polygonColor(SDL_Surface *dst, const Sint16 *vx, const Sint16 *vy, int n, Uint32 color);
int polygonRGBA(SDL_Surface *dst, const Sint16 *vx, const Sint16 *vy, int n, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int aapolygonColor(SDL_Surface *dst, const Sint16 *vx, const Sint16 *vy, int n, Uint32 color);
int aapolygonRGBA(SDL_Surface *dst, const Sint16 *vx, const Sint16 *vy, int n, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int _gfxPrimitivesCompareInt(const void *a, const void *b);
int filledPolygonColorMT(SDL_Surface *dst, const Sint16 *vx, const Sint16 *vy, int n, Uint32 color, int **polyInts, int *polyAllocated);
int filledPolygonRGBAMT(SDL_Surface *dst, const Sint16 *vx, const Sint16 *vy, int n, Uint8 r, Uint8 g, Uint8 b, Uint8 a, int **polyInts, int *polyAllocated);
int filledPolygonColor(SDL_Surface *dst, const Sint16 *vx, const Sint16 *vy, int n, Uint32 color);
int filledPolygonRGBA(SDL_Surface *dst, const Sint16 *vx, const Sint16 *vy, int n, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int _HLineTextured(SDL_Surface *dst, Sint16 x1, Sint16 x2, Sint16 y, SDL_Surface *texture, int texture_dx, int texture_dy);
int texturedPolygonMT(SDL_Surface *dst, const Sint16 *vx, const Sint16 *vy, int n, SDL_Surface *texture, int texture_dx, int texture_dy, int **polyInts, int *polyAllocated);
int texturedPolygon(SDL_Surface *dst, const Sint16 *vx, const Sint16 *vy, int n, SDL_Surface *texture, int texture_dx, int texture_dy);
double _evaluateBezier(double *data, int ndata, double t);
int bezierColor(SDL_Surface *dst, const Sint16 *vx, const Sint16 *vy, int n, int s, Uint32 color);
int bezierRGBA(SDL_Surface *dst, const Sint16 *vx, const Sint16 *vy, int n, int s, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
int thickLineColor(SDL_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 width, Uint32 color);
int thickLineRGBA(SDL_Surface *dst, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Uint8 width, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/* toolkit/SDL_rotozoom.c */
Uint32 _colorkey(SDL_Surface *src);
int _shrinkSurfaceRGBA(SDL_Surface *src, SDL_Surface *dst, int factorx, int factory);
int _shrinkSurfaceY(SDL_Surface *src, SDL_Surface *dst, int factorx, int factory);
int _zoomSurfaceRGBA(SDL_Surface *src, SDL_Surface *dst, int flipx, int flipy, int smooth);
int _zoomSurfaceY(SDL_Surface *src, SDL_Surface *dst, int flipx, int flipy);
void _transformSurfaceRGBA(SDL_Surface *src, SDL_Surface *dst, int cx, int cy, int isin, int icos, int flipx, int flipy, int smooth);
void transformSurfaceY(SDL_Surface *src, SDL_Surface *dst, int cx, int cy, int isin, int icos, int flipx, int flipy);
SDL_Surface *rotateSurface90Degrees(SDL_Surface *src, int numClockwiseTurns);
void _rotozoomSurfaceSizeTrig(int width, int height, double angle, double zoomx, double zoomy, int *dstwidth, int *dstheight, double *canglezoom, double *sanglezoom);
void rotozoomSurfaceSizeXY(int width, int height, double angle, double zoomx, double zoomy, int *dstwidth, int *dstheight);
void rotozoomSurfaceSize(int width, int height, double angle, double zoom, int *dstwidth, int *dstheight);
SDL_Surface *rotozoomSurface(SDL_Surface *src, double angle, double zoom, int smooth);
SDL_Surface *rotozoomSurfaceXY(SDL_Surface *src, double angle, double zoomx, double zoomy, int smooth);
void zoomSurfaceSize(int width, int height, double zoomx, double zoomy, int *dstwidth, int *dstheight);
SDL_Surface *zoomSurface(SDL_Surface *src, double zoomx, double zoomy, int smooth);
SDL_Surface *shrinkSurface(SDL_Surface *src, int factorx, int factory);

/* toolkit/text.c */
void text_init();
void text_deinit();
void text_offset_set(int x, int y);
void text_offset_reset();
void text_color_set(int r, int g, int b);
void text_set_selection(sint64 *start, sint64 *end, uint8 *started);
const char *get_font_filename(int font);
int get_font_id(const char *name, size_t size);
char *text_strip_markup(char *buf, size_t *buf_len, uint8 do_free);
int text_color_parse(const char *color_notation, SDL_Color *color);
int blt_character(int *font, int orig_font, SDL_Surface *surface, SDL_Rect *dest, const char *cp, SDL_Color *color, SDL_Color *orig_color, uint64 flags, SDL_Rect *box, int *x_adjust);
int glyph_get_width(int font, char c);
int glyph_get_height(int font, char c);
void string_blt(SDL_Surface *surface, int font, const char *text, int x, int y, const char *color_notation, uint64 flags, SDL_Rect *box);
void string_blt_shadow(SDL_Surface *surface, int font, const char *text, int x, int y, const char *color_notation, const char *color_shadow_notation, uint64 flags, SDL_Rect *box);
void string_blt_format(SDL_Surface *surface, int font, int x, int y, const char *color_notation, uint64 flags, SDL_Rect *box, const char *text, ...) __attribute__((format(printf, 8, 9)));
void string_blt_shadow_format(SDL_Surface *surface, int font, int x, int y, const char *color_notation, const char *color_shadow_notation, uint64 flags, SDL_Rect *box, const char *text, ...) __attribute__((format(printf, 9, 10)));
int string_get_width(int font, const char *text, uint64 flags);
int string_get_height(int font, const char *text, uint64 flags);
void string_truncate_overflow(int font, char *text, int max_width);
void text_enable_debug();

/* toolkit/text_input.c */
int text_input_center_offset();
void text_input_draw_background(SDL_Surface *surface, int x, int y, int bitmap);
void text_input_draw_text(SDL_Surface *surface, int x, int y, int font, const char *text, const char *color_notation, uint64 flags, int bitmap, SDL_Rect *box);
void text_input_show(SDL_Surface *surface, int x, int y, int font, const char *text, const char *color_notation, uint64 flags, int bitmap, SDL_Rect *box);
void text_input_clear();
void text_input_open(int maxchar);
void text_input_history_clear();
void text_input_add_string(const char *text);
int text_input_handle(SDL_KeyboardEvent *key);

/* toolkit/tooltip.c */
void tooltip_create(int mx, int my, int font, const char *text);
void tooltip_show();

/* toolkit/widget.c */
void init_widgets_fromCurrent();
widgetdata *create_widget_object(int widget_subtype_id);
void remove_widget_object(widgetdata *widget);
void remove_widget_object_intern(widgetdata *widget);
void remove_widget_inv(widgetdata *widget);
void init_widgets();
void kill_widgets();
void reset_widget(const char *name);
void widgets_ensure_onscreen();
void kill_widget_tree(widgetdata *widget);
widgetdata *create_widget(int widget_id);
void remove_widget(widgetdata *widget);
void detach_widget(widgetdata *widget);
void save_interface_file();
void save_interface_file_rec(widgetdata *widget, FILE *stream);
int widget_event_mousedn(int x, int y, SDL_Event *event);
int widget_event_mouseup(int x, int y, SDL_Event *event);
int widget_event_mousemv(int x, int y, SDL_Event *event);
int widget_event_start_move(widgetdata *widget, int x, int y);
int widget_event_respond(int x, int y);
int widget_event_override();
widgetdata *get_widget_owner(int x, int y, widgetdata *start, widgetdata *end);
widgetdata *get_widget_owner_rec(int x, int y, widgetdata *widget, widgetdata *end);
void process_widgets();
void process_widgets_rec(widgetdata *widget);
void SetPriorityWidget(widgetdata *node);
void SetPriorityWidget_reverse(widgetdata *node);
void insert_widget_in_container(widgetdata *widget_container, widgetdata *widget);
widgetdata *get_outermost_container(widgetdata *widget);
widgetdata *widget_find_by_surface(SDL_Surface *surface);
void move_widget(widgetdata *widget, int x, int y);
void move_widget_rec(widgetdata *widget, int x, int y);
void resize_widget(widgetdata *widget, int side, int offset);
void resize_widget_rec(widgetdata *widget, int x, int width, int y, int height);
widgetdata *add_label(char *text, int font, const char *color);
widgetdata *add_bitmap(int bitmap_id);
widgetdata *create_menu(int x, int y, widgetdata *owner);
void add_menuitem(widgetdata *menu, char *text, void (*menu_func_ptr)(widgetdata *, int, int), int menu_type, int val);
void add_separator(widgetdata *widget);
void menu_finalize(widgetdata *widget);
void widget_redraw_all(int widget_type_id);
void menu_move_widget(widgetdata *widget, int x, int y);
void menu_create_widget(widgetdata *widget, int x, int y);
void menu_remove_widget(widgetdata *widget, int x, int y);
void menu_detach_widget(widgetdata *widget, int x, int y);
void menu_set_say_filter(widgetdata *widget, int x, int y);
void menu_set_shout_filter(widgetdata *widget, int x, int y);
void menu_set_gsay_filter(widgetdata *widget, int x, int y);
void menu_set_tell_filter(widgetdata *widget, int x, int y);
void menu_set_channel_filter(widgetdata *widget, int x, int y);
void submenu_chatwindow_filters(widgetdata *widget, int x, int y);
void menu_inv_filter_all();
void menu_inv_filter_applied();
void menu_inv_filter_containers();
void menu_inv_filter_magical();
void menu_inv_filter_cursed();
void menu_inv_filter_unidentified();
void menu_inv_filter_locked();
void menu_inv_filter_unapplied();
#endif
