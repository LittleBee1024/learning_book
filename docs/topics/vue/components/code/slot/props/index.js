const TodoList = {
   data() {
      return {
         items: ['Feed a cat', 'Buy milk']
      }
   },
   template: `
      <ul>
         <li v-for="(item, index) in items">
            <slot :item="item" :index="index">{{item}}</slot>
         </li>
      </ul>
   `
}

const RootComponent = {
   components: {
      'todo-list': TodoList
   }
}

const vm = Vue.createApp(RootComponent).mount('#app')
