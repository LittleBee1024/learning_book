import vHeader from './header/index.js'
import vFooter from './footer/index.js'
import vHome from './home/index.js'
import vAbout from './about/index.js'

const vAside = {
   props: ['routes'],
   emits: ['click'],
   template: `
      <el-menu>
         <template v-for="(route, i) in routes">
            <el-menu-item :index="i.toString()" @click="$emit('click', route)">{{ route }}</el-menu-item>
         </template>
      </el-menu>
   `
}

const vMain = {
   data() {
      return {
         pages: {
            'Home': vHome,
            'About': vAbout
         },
         NotFoundPage: { template: '<p>Page not found</p>' }
      }
   },
   props: ['currentRoute'],
   computed: {
      CurrentComponent() {
         return this.pages[this.currentRoute] || this.NotFoundPage
      }
   },

   render() {
      return Vue.h(this.CurrentComponent)
   }
}

const vRoot = {
   data() {
      return {
         title: "Demo Header",
         footer: "Demo Footer",
         routes: ["Home", "About"],
         currentRoute: "Home"
      }
   },
   components: {
      'vheader': vHeader,
      'vmain': vMain,
      'vaside': vAside,
      'vfooter': vFooter
   },
   methods: {
      go(route) {
         this.currentRoute = route
      }
   },
   template: `
      <div class='vroot'>
         <el-container>
            <el-header> <vheader :title="title" /> </el-header>
            <el-container>
               <el-aside width="200px"> <vaside :routes="routes" @click="go"/> </el-aside>
               <el-container>
                  <el-main> <vmain :current-route="currentRoute" /> </el-main>
                  <el-footer> <vfooter :footer="footer"/> </el-footer>
               </el-container>
            </el-container>
         </el-container>
      </div>
   `
}

export default vRoot;
