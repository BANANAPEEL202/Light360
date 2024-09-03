import spotipy
from spotipy.oauth2 import SpotifyOAuth
from colorthief import ColorThief
import urllib.request
import os 
from dotenv import load_dotenv

load_dotenv("./spotify_secrets.env")
CLIENT_ID = os.getenv('CLIENT_ID')
CLIENT_SECRET = os.getenv('CLIENT_SECRET')
REDIRECT_URI = os.getenv('REDIRECT_URI')

scope = 'user-read-currently-playing'

spotify = spotipy.Spotify(auth_manager=SpotifyOAuth(client_id=CLIENT_ID,
                                               client_secret=CLIENT_SECRET,
                                               redirect_uri=REDIRECT_URI,
                                               scope=scope))

def get_colors(image_url, track_name, num_colors):
    file_path = "./cache/" + track_name + ".jpg"
    urllib.request.urlretrieve(image_url, file_path)
    color_thief = ColorThief(file_path)
    palette = color_thief.get_palette(color_count=num_colors, quality=1)
    return palette

def get_current_song():
    # Get track information
    current_track = spotify.current_user_playing_track()
    track_id = current_track["item"]["id"]
    track_name = current_track['item']['name']
    artist_name = current_track['item']['artists'][0]['name']
    album_cover_url = current_track['item']['album']['images'][0]['url']
    colors = get_colors(album_cover_url, track_name, 2)
    print(colors)


get_current_song()